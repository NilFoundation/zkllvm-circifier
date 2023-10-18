#!/usr/bin/env python

import os
import re
import glob
import shutil
import argparse
import json
import subprocess
import tempfile
from contextlib import contextmanager

@contextmanager
def cd(newdir):
  prevdir = os.getcwd()
  os.chdir(os.path.expanduser(newdir))
  try:
    yield
  finally:
    os.chdir(prevdir)

def execute(cmdline, verbose=False):
  if verbose:
    print(' '.join(cmdline))
  try:
    return subprocess.check_output(cmdline)
  except subprocess.CalledProcessError as e:
    print(e.output)
    os.sys.exit(1)

def get_path(opt, optname, varname, default):
  if opt:
    return opt
  var = os.environ.get(varname)
  if var:
    return var
  if os.path.exists(default):
    return default
  print('Use {} option or set {} environment variable'.format(optname,
    varname))
  os.sys.exit(1)

extensions = ['.c', '.cpp', '.cxx', '.ll', '.bc', '.s', '.S']
parser = argparse.ArgumentParser('tvm-build++.py',
  description='Tool for building a C++ contract for the TON virtual machine')

named = parser.add_argument_group('required named arguments')
named.add_argument('-A', '--abi', required=True, help='ABI description file')
parser.add_argument('inputs', metavar='file', nargs='+',
                    help=', '.join(extensions) + ' files')

parser.add_argument('-v', '--verbose', action='store_true', default=False,
                    help='print command lines of child processes')
parser.add_argument('--save-temps', action='store_true', default=False,
                    help='store intermediate files in the working directory and don\'t delete them')
parser.add_argument('-o', '--output', help='bag-of-cells output file name')
parser.add_argument('--cflags', help='flags and options for C frontend')
parser.add_argument('--cxxflags', help='flags and options for C++ frontend')
parser.add_argument('--linkerflags', help='flags for tvm_linker')
parser.add_argument('--opt-flags', help='flags and options for LLVM optimizer')
parser.add_argument('-S', '--asm-only', action='store_true', default=False,
                    help='produce assembler output')
parser.add_argument('--inline-loads-stores', action='store_true', default=False,
                    help='experimental inlining of loads/stores')

parser.add_argument('--llvm-bin', help='path to LLVM binaries directory')
parser.add_argument('--linker', help='path to TVM linker executable')
parser.add_argument('--stdlib', help='path to standard library directory')
parser.add_argument('--include', help='path to standard include directory')

args = parser.parse_args()

bindir = os.path.dirname(os.path.realpath(__file__))
if not os.path.exists(os.path.join(bindir, 'llvm-link')):
  bindir = 'non-existent'

tvm_llvm_bin = get_path(args.llvm_bin, '--llvm-bin', 'TVM_LLVM_BINARY_DIR', bindir)
tvm_linker = get_path(args.linker, '--linker', 'TVM_LINKER', os.path.join(bindir, 'tvm_linker'))
tvm_stdlib = get_path(args.stdlib, '--stdlib', 'TVM_LIBRARY_PATH', os.path.join(bindir, '../lib'))
tvm_include = get_path(args.include, '--include', 'TVM_INCLUDE_PATH', os.path.join(bindir, '../include'))
tvm_sysroot = os.path.join(bindir, "..");

input_c   = []
input_cpp = []
input_ll  = []
input_bc  = []
input_asm = []

for filename in args.inputs:
  _, ext = os.path.splitext(filename)
  if ext == '.c':
    input_c += [filename]
  elif ext in ['.cc', '.cpp', '.cxx']:
    input_cpp += [filename]
  elif ext == '.ll':
    input_ll += [filename]
  elif ext == '.bc':
    input_bc += [filename]
  elif ext == '.s' or ext == '.S':
    input_asm += [filename]
  else:
    print('Unsupported input file extension: ' + filename)
    print('Supported extensions: ' + ', '.join(extensions))
    os.sys.exit(1)

cxxflags = ['-O3']
if args.cxxflags:
  cxxflags += args.cxxflags.split()

pwd = os.path.abspath(os.getcwd()) + '/'

for filename in input_cpp:
  if args.save_temps:
    tmp_file = pwd + '1-clang.ll'
  else:
    _, tmp_file = tempfile.mkstemp()
  execute([os.path.join(tvm_llvm_bin, 'clang++'), '-target', 'tvm'] +
    cxxflags + ['-S', '-emit-llvm', filename, '-o', tmp_file, '--sysroot=' + tvm_sysroot], args.verbose)
  input_bc += [tmp_file]

cflags = ['-O1']
if args.cflags:
  cflags += args.cflags.split()

for filename in input_c:
  _, tmp_file = tempfile.mkstemp()
  execute([os.path.join(tvm_llvm_bin, 'clang'), '-target', 'tvm', '-isystem', tvm_include] +
    cflags + ['-S', '-emit-llvm', filename, '-o', tmp_file], args.verbose)
  input_bc += [tmp_file]

for filename in input_ll:
  _, tmp_file = tempfile.mkstemp()
  execute([os.path.join(tvm_llvm_bin, 'llvm-as'), filename, '-o',
    tmp_file], args.verbose)
  input_bc += [tmp_file]

if args.save_temps:
  bitcode = pwd + '2-llvm-link.ll'
else:
  _, bitcode = tempfile.mkstemp()
execute([os.path.join(tvm_llvm_bin, 'llvm-link')] + input_bc +
  ['-S', '-o', bitcode], args.verbose)

entry_points = [ "main_external", "main_internal", "main_ticktock", "main_split", "main_merge", "get_method" ]

replace_loads_stores = []
if args.inline_loads_stores:
  replace_loads_stores = ['-tvm-load-store-replace']

if args.save_temps:
  bitcode_int = pwd + '3-opt.ll'
else:
  _, bitcode_int = tempfile.mkstemp()
execute([os.path.join(tvm_llvm_bin, 'opt'), bitcode, '-S', '-o', bitcode_int] +
  replace_loads_stores + ['-internalize', '-internalize-public-api-list=' +
  ','.join(entry_points)], args.verbose)

if args.opt_flags:
  opt_flags = args.opt_flags.split()
else:
  opt_flags = ['-O3']

if args.save_temps:
  bitcode_opt = pwd + '4-opt_O3.ll'
else:
  _, bitcode_opt = tempfile.mkstemp()
execute([os.path.join(tvm_llvm_bin, 'opt')] + opt_flags + ['-S', bitcode_int, '-o',
  bitcode_opt], args.verbose)

if args.save_temps:
  asm = pwd + '5-llc.asm'
else:
  _, asm = tempfile.mkstemp()
execute([os.path.join(tvm_llvm_bin, 'llc'), '-march', 'tvm', bitcode_opt,
  '-o', asm], args.verbose)

if input_asm:
  if args.verbose:
    print('cat ' + ' '.join(input_asm) + ' >>' + asm)
  with open(asm, 'a') as asmfile:
    for fname in input_asm:
      with open(fname) as infile:
        asmfile.write(infile.read())

if args.asm_only:
  if args.output:
    if args.verbose:
      print('cp ' + asm + ' ' + args.output)
    shutil.copy2(asm, args.output)
  else:
    fname, _ = os.path.splitext(args.abi)
    if args.verbose:
      print('cp ' + asm + ' ' + fname + '.s')
    shutil.copy2(asm, fname + '.s')
  print('Build succeeded.')
  os.sys.exit(0)

if not args.output:
  output = os.getcwd()
elif os.path.isabs(args.output):
  output = args.output
else:
  output = os.path.join(os.getcwd(), args.output)
abi_path = os.path.abspath(args.abi)

if args.save_temps:
  tmpdir = './'
else:
  tmpdir = tempfile.mkdtemp()
  if args.verbose:
    print('cd ' + tmpdir)

with cd(tmpdir):
  linkerflags = []
  if args.linkerflags:
    linkerflags = args.linkerflags.split()
  if args.output:
    linkerflags.extend(['-o', args.output])
  execute([tvm_linker, 'compile', asm, '--lib', os.path.join(tvm_stdlib,
    'stdlib_cpp.tvm'), '--abi-json', abi_path] + linkerflags, args.verbose)
  if not args.save_temps:
    for filename in glob.glob('*'):
      if args.verbose:
        print('cp ' + filename + ' ' + output)
      shutil.copy2(filename, output)

  if args.output:
    out = execute([tvm_linker, 'decode', args.output, '--tvc'])
    for line in out.decode("utf-8").split("\n"):
      if match := re.match(" code: (.*)$", line):
        code = match[1]
        break
    # Save code in separate file
    with open(f"{args.output}.code", "w") as f:
      f.write(code)

print('Build succeeded.')

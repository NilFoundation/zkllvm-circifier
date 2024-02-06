#!/usr/bin/env ruby

require 'json'
require 'keccak256'
require 'open3'
require 'optparse'
require 'ostruct'
require 'yaml'

$tests_count = 0
$files_count = 0

$options = OpenStruct.new
OptionParser.new do |opts|
  opts.banner = 'Usage: evm-runner.rb [options] FILE'
  opts.on('--glob=STRING', 'Glob to find test files')
  opts.on('--src-dir=PATH', 'Path to LLVM source directory') { |x| $options.src_dir = x }
  opts.on('--bin-dir=PATH', 'Path to LLVM binary directory') { |x| $options.bin_dir = x }
  opts.on('--clang-args=ARGS', 'Command line arguments for clang') { |x| $options.clang_args = x }
  opts.on('--linker-args=ARGS', 'Command line arguments for linker') { |x| $options.linker_args = x }
  opts.on('--stdlib=PATH', 'Path to EVM stdlib') { |x| $options.stdlib = x }
  opts.on('--debug', 'Produce debug information')
  opts.on('--save-temps', 'Pass compiler option `-save-temps`') { $options.save_temps = _1 }
  opts.on('--no-compile', 'Don\'t compile anything, assume it is already compiled') { $options.no_compile = true }
  opts.on('--no-ctor', 'Do not generate EVM constructor') { $options.no_ctor = true }
  opts.on('--no-stdlib', 'Do not use stdlib') { $options.no_stdlib = true }
  opts.on('--evmone=PATH', 'Path to evmone install directory, which contains `bin/evmc` and `lib/libevmone.so`')
  opts.on('-v', '--verbose', 'Verbose logging')
end.parse!(into: $options)

def options; $options; end

options.src_dir = File.realpath(__dir__ + '/../../../') unless options.src_dir

def command(cmd)
  puts "COMMAND: #{cmd}" if options.verbose
  output, status = Open3.capture2e(cmd.to_s)
  if status.signaled? || status.exitstatus != 0
    raise "Command failed '#{cmd}':\n#{output}"
  end
  puts output if options.verbose
  output
end

def collect_runs(filename)
  runs = []
  File.readlines(filename).each do |line|
    data = YAML.load("{ #{line.split('EVM_RUN:')[1]} }")
    data = JSON.parse(data.to_json, object_class: OpenStruct).freeze
    runs << data if line.start_with? '// EVM_RUN:'
  end
  runs
end

def get_func_keccak(name, abifile)
  abi = JSON.load(File.read(abifile))
  dscr = abi.select { |x| x['name'].split('(', 2)[0] == name }
  raise "Function not found: #{name}" if dscr.empty?
  raise "More than one function found with name: #{name}" if dscr.size > 1
  dscr = dscr[0]
  inputs = dscr['inputs'].map { |x| x['type'] }
  signature = "#{name}(#{inputs.join(',')})"
  function_sha3 = Digest::Keccak256.new.hexdigest(signature)
  function_sha3
end

def run_vm(codefile, input=nil)
  input = input ? "--input #{input}" : ""
  if options.evmone
    result = command("#{options.evmone}/bin/evmc run @#{codefile} --vm #{options.evmone}/lib/libevmone.so,trace-bin #{input}").strip
    m = result.match /Output: +(.*)$/
    raise "Invalid evmc output: #{result}" unless m
    return m[1]
  end
  result = command("evm #{input} --codefile #{codefile} run").strip
  result[2..-1]
end

def run_test(source)
  puts "Run tests for: #{source}" if options.verbose
  runs = collect_runs(source)
  if runs.empty?
    puts "Skipping, since file doesn't contain any test case" if options.verbose
    return
  end
  bindir = File.realpath(options.bin_dir)
  basename = File.basename(source, '.*')
  abifile = "#{basename}.abi"
  codefile = "#{basename}.evm_h"

  unless options.no_compile
    clang_cmd = "#{bindir}/ecc #{source} -o #{codefile} "
    clang_cmd += ' -v ' if options.verbose
    # Uncomment to disable llvm optimizations
    #clang_cmd += ' -Xclang -disable-llvm-passes '
    clang_cmd += ' -std=c++17 '
    clang_cmd += ' -O3 '
    clang_cmd += ' -DNDEBUG '
    clang_cmd += ' -Xlinker --no-ctor ' if options.no_ctor
    clang_cmd += " -Xlinker #{options.linker_args}" if options.linker_args
    clang_cmd += " #{options.clang_args}" if options.clang_args
    clang_cmd += " -nostdlib" if options.no_stdlib
    clang_cmd += " -save-temps" if options.save_temps

    command(clang_cmd)
  end

  # Run constructor if required
  unless options.no_ctor
    result = run_vm(codefile)
    # Write the real code of the contract
    codefile = "#{basename}_deployed.evm_h"
    File.write(codefile, result)
  end

  $files_count += 1

  runs.each_with_index do |test_run, index|
    puts " [#{index}] Testcase: #{test_run.to_h}" if options.verbose

    raise "`function` must be specified in test case" unless test_run.function
    function_sha3 = get_func_keccak(test_run.function, abifile)
    input = function_sha3[0..7]
    # Use 'x & 0xfffff...' for proper extraction hex strings from negative numbers
    mask = (1 << 256) - 1
    if test_run.input.is_a? Array
      input += test_run.input.map{|x| "%064x" % (x & mask) }.join
    elsif test_run.input.is_a? String
      input += test_run.input
    else
      raise "Wrong input type: #{test_run.input.class}"
    end if test_run.input

    result = run_vm(codefile, input).to_i(16)
    if test_run.result && test_run.result != result
      if options.debug
        result = command("evm -debug --gas 1000000 --input #{input} --codefile #{codefile} run")
        File.write("#{basename}.run", result)
      end
      raise "Wrong output result: expected(#{test_run.result}) != real(#{result})"
    end
    $tests_count += 1
  end
  puts "Passed" if options.verbose
end

def main
  raise "`--glob` option must be specified" unless options.glob
  Dir.glob(options.glob) { |file| run_test(file) }
  puts "#{$tests_count} tests passed from #{$files_count} files"
end

main
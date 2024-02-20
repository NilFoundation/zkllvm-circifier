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
  opts.banner = 'Usage: evm_test.rb [options] FILE'
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
  opts.on('--vmrun=PATH', 'Path to vmrun tool')
  opts.on('-v', '--verbose', 'Verbose logging')
end.parse!(into: $options)

def options; $options; end

VM_RUN = options.vmrun # "/home/mike/bld/dbms-nix/bin/vmrun/vm_run"
ECC_GEN = "#{__dir__}/../evm-sdk/ecc_gen.rb"

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
    if line.start_with? '// EVM_RUN:'
      data = YAML.load("{ #{line.split('EVM_RUN:')[1]} }")
      data = JSON.parse(data.to_json, object_class: OpenStruct).freeze
      runs << data
    end
  end
  runs
end

def pack_i256(n)
  mask = (1 << 64) - 1
  (0..3).reverse_each.reduce([]) { |res, i| res << ((n >> 64 * i) & mask) }.pack("Q>Q>Q>Q>")
end

def unpack_i256(s)
  (0..3).reduce(0) { |res, i| res |= (s[i * 8..].unpack("Q>")[0] << (3 - i) * 64) }
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
  runs = collect_runs(source)
  return false if runs.empty?

  puts "Run tests for: #{source}" if options.verbose
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
  true
end

class Checker

  def initialize(source_file)
    @source_file = source_file
    @code = nil

    @bindir = File.realpath(options.bin_dir)
    @basename = File.basename(source_file, '.*')
    @abifile = "#{@basename}.abi"
    @codefile = "#{@basename}.evm_h"
  end

  def run
    actions = File.readlines(@source_file)
                  .map(&:strip)
                  .each_with_index.select { _1[0].start_with?("//! ") }
                  # Remove prefix '//!'
                  .map { |x, i| [x[3..], i + 1] }
                  # Add extra {} for empty arguments cases, like "DEPLOY". Because Ruby treats it as a constant.
                  .map { |x, i| [x.split.size <=1 ? x + " {}" : x, i] }

    return false if actions.empty?

    unless VM_RUN
      puts "WARNING: File contains vmrun tests, but `--vmrun` option is not specified. File: #{@source_file}"
      return true
    end

    compile

    actions.each do |action|
      self.instance_eval action[0], @source_file, action[1]
    end
    $tests_count += 1
    $files_count += 1
    true
  end

  def compile

    command("ruby #{ECC_GEN} -c #{@source_file}")

    clang_cmd = "#{@bindir}/ecc #{@source_file} -o #{@codefile} "
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
    clang_cmd += " -I./"

    command(clang_cmd)

    @code = "EVM".unpack("H*").first + File.read(@codefile)
  end

  def DEPLOY(**args)
    address = args[:address] || 0
    command("#{VM_RUN} deploy --address #{args['address'] or 0} --code-hex #{@code}")
  end

  def CALL(**args)
    function = args[:function]&.to_s
    address = args[:address] || 0
    result = args[:result]
    input = args[:input]

    raise "Function name must be specified" unless function
    function_sha3 = get_func_keccak(function, @abifile)
    calldata = "" #function_sha3[0..7]
    # Use 'x & 0xfffff...' for proper extraction hex strings from negative numbers
    mask = (1 << 256) - 1
    if input.is_a? Array
      calldata += input.map{|x| "%064x" % (x & mask) }.join
    elsif input.is_a? String
      calldata += input
    else
      raise "Wrong input type: #{input.class}"
    end if input

    selector = function_sha3[0..7].to_i(16)
    output = command("#{VM_RUN} call --method-id #{selector} --address #{address} --args s#{calldata} --verbose")
    if result
      m = output.match /Result stack:  (.*)/
      raise "Invalid vm_run output: #{output}" unless m
      stack = eval(m[1])
      raise "Must be only one stack value" unless stack.size == 1
      raise "Result mismatch: expected(#{result}) != real(#{stack[0]})" if result != stack[0]
    end
  end

  def CHECK_STOR(**args)
    address = args[:address] || 0
    key = args[:key]
    result = args[:result] || 0
    raise "Key must be specified" unless key

    def calc_key(key, hex_out=true)
      # return key if key.is_a? Integer
      s = key.reduce('') do |acc, key|
        acc + (key.is_a?(Array) ? calc_key(key, false) : pack_i256(key))
      end
      hex_out ? Digest::Keccak256.new.hexdigest(s) : Digest::Keccak256.new.digest(s)
    end

    if key.is_a? String
      key = self.instance_eval(key)
    elsif key.is_a? Array
      key = "0x" + calc_key(key)
    else
      raise "Invalid type of key argument: #{key.class}" unless key.is_a? Integer
    end

    output = command("#{VM_RUN} storage-get --key #{key} --address #{address}").to_i
    raise "Result mismatch: expected(#{result}) != real(#{output})" if result != output
  end

  def sha3(*args)
    data = args.reduce('') { |acc, key| acc + pack_i256(key) }
    unpack_i256(Digest::Keccak256.new.digest(data))
  end
end

def main
  raise "`--glob` option must be specified" unless options.glob
  Dir.glob(options.glob) do |file|
    puts "Run tests for: #{file}" if options.verbose
    tested1 = run_test(file)
    tested2 = Checker.new(file).run
    raise "File doesn't contain any tests: #{file}" unless tested1 || tested2
  end
  puts "#{$tests_count} tests passed from #{$files_count} files"
end

main
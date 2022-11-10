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
  opts.on('--bin-dir=PATH', 'Path to LLVM binary directory') { |x| $options.bin_dir = x }
  opts.on('-v', '--verbose', 'Verbose logging')
end.parse!(into: $options)

def options; $options; end

def command(cmd)
  puts "COMMAND: #{cmd}" if $options.verbose
  output, status = Open3.capture2e(cmd.to_s)
  if status.signaled? || status.exitstatus != 0
    raise "Command failed '#{cmd}':\n#{output}"
  end
  puts output if $options.verbose
  output.strip!
  output = output.to_i(16) if output.start_with? '0x'
  output
end

def load_run_info(line)
  data = YAML.load("{ #{line.split('EVM_RUN:')[1]} }")
  JSON.parse(data.to_json, object_class: OpenStruct).freeze
end

def collect_runs(filename)
  runs = []
  File.readlines(filename).each do |line|
    runs << load_run_info(line) if line.include? 'EVM_RUN:'
  end
  runs
end

def run_test(source)
  puts "Run tests for: #{source}" if $options.verbose
  runs = collect_runs(source)
  if runs.empty?
    puts "Skipping, since file doesn't contain any test case" if $options.verbose
    return
  end
  bindir = File.realpath($options.bin_dir)
  basename = File.basename(source, '.*')
  codefile = "#{basename}.ebi.txt"
  command("#{bindir}/clang -target evm -O3 #{source} -o #{basename}.ebi -v -Xclang -disable-llvm-passes -fno-exceptions")
  command("xxd -p -c 0 #{basename}.ebi  #{codefile}")

  # Add extra case to check the function selector when calling of an absent function
  runs << OpenStruct.new({function: "@@@wrong@@@", result: "error: execution reverted"})

  $files_count += 1

  runs.each do |test_run|
    puts "  Testcase: #{test_run.to_h}" if $options.verbose
    raise "`function` must be specified in test case" unless test_run.function
    function_sha3 = Digest::Keccak256.new.hexdigest(test_run.function)
    input = function_sha3[0..7]
    input += test_run.input.map{|x| "%064x" % x }.join if test_run.input
    result = command("evm --input #{input} --codefile #{codefile} run")
    if test_run.result && test_run.result != result
      raise "Wrong output result: #{test_run.result} != #{result}"
    end
    $tests_count += 1
  end
  puts "Passed" if $options.verbose
end

def main
  raise "`--glob` option must be specified" unless options.glob
  Dir.glob(options.glob) { |file| run_test(file) }
  puts "#{$tests_count} tests passed from #{$files_count} files"
end

main
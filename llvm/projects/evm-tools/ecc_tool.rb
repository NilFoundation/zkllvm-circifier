#!/usr/bin/env ruby
# frozen_string_literal: true

require 'erb'
require 'open3'
require 'optparse'
require 'ostruct'
require 'pathname'
require 'yaml'

$options = OpenStruct.new
OptionParser.new do |opts|
  opts.banner = 'Usage: ecc_gen.rb [options] FILE'
  opts.on('--output=PATH', 'Path to output file')
  opts.on('--ecc=PATH', 'Path to ecc compiler')
  opts.on('--compile', 'Compile file')
  opts.on('--generate=STRING', 'Select component to generate: host, contract, python')
  opts.on('--data-file=PATH', 'Path to the contract data file in yaml format') { $options.data_file = _1 }
  opts.on('--clang-args=ARGS', 'Command line arguments for clang') { $options.clang_args = _1 }
  opts.on('--linker-args=ARGS', 'Command line arguments for linker') { $options.linker_args = _1 }
  opts.on('--no-ctor', 'Do not generate EVM constructor') { $options.no_ctor = true }
  opts.on('-c', '--cpp-input', 'Input file is a c++ file with embedded storage description') { $options.cpp_input = _1 }
  opts.on('-v', '--verbose', 'Verbose logging')
end.parse!(into: $options)
def options; $options; end

INPUT_FILE=ARGV.pop
raise "Input file must be provided" unless INPUT_FILE

def command(cmd)
  puts "COMMAND: #{cmd}" if options.verbose
  output, status = Open3.capture2e(cmd.to_s)
  if status.signaled? || status.exitstatus != 0
    raise "Command failed '#{cmd}':\n#{output}"
  end
  puts output if options.verbose
  output
end

def cpp_type(type)
  @type_aliases ||= {"u8" => "uint8_t", "i8" => "int8_t", "u16" => "uint16_t", "i16" => "int16_t",
                     "u32" => "uint32_t", "i32" => "int32_t", "u64" => "uint64_t", "i64" => "int64_t",
                     "u128" => "uint128_t", "i128" => "int128_t", "u256" => "uint256_t", "i256" => "int256_t"}
  @type_aliases[type] || type
end

class Type
  attr_reader :name

  def initialize(name)
    @name = name
  end

  def bytes_size
    raise "Abstract type hasn't bytes size"
  end

  def to_s; @name; end
  def cpp_name; cpp_type(@name); end
  def storage_name; cpp_type(@name); end
  def python_name
    @name =~ /[iu]\d+/ ? 'int' : @name
  end
end

class PrimitiveType < Type
  def initialize(name)
    super(name)
  end

  def bits
    return 8 if @name == "bool"
    m = @name.match /[ui](\d+)/
    raise "type_bits: can't parse type: #{@name}" unless m
    m[1].to_i
  end

  def bytes_size
    bs = bits
    raise "bytes_size: bits is not multiple of 8: #{bs}" unless bs % 8 == 0
    bs / 8
  end

  def storage_name
    "Slot<#{cpp_type(@name)}>"
  end

  def storage_name_static(index)
    index = index.call if index.is_a?(Proc)
    "SlotStatic<#{cpp_type(@name)}, #{index}>"
  end

  def slots_size
    1
  end
end

class StructType < Type
  attr_reader :fields, :descr

  def initialize(name, descr, fields)
    super(name)
    @fields = fields
    @descr = descr
  end

  def bytes_size
    @fields.sum do |field|
      sz = field.type.slots_size
      return nil if sz.nil?
      sz
    end
  end

  def to_s
    "#{@name}: #{@fields.map(&:name).join(', ')}"
  end

  def slots_size
    @size ||= @fields.sum { _1.type.slots_size }
  end

  def storage_name
    @name
  end

  def storage_name_static(index)
    index = index.call if index.is_a?(Proc)
    "#{@name}Static<#{index}>"
  end
end


class ContainerType < Type
  attr_reader :base_type_name, :sub_type

  PYTHON_NAMES_MAP = {'Vector' => 'list', 'Map' => 'dict', 'IterableMap' => 'dict'}
  PYTHON_NAMES_MAP.default_proc = -> (_, k) { raise KeyError, "Key `#{k}` not found!" }

  def initialize(name, base_type_name, sub_type)
    super(name)
    @base_type_name = base_type_name
    @sub_type = sub_type
  end

  def slots_size
    @base_type_name == 'IterableMap' ? 4 : 1
  end

  def cpp_name
    "#{@base_type_name}<#{@sub_type.cpp_name}>"
  end

  def storage_name
    "#{@base_type_name}<#{@sub_type.cpp_name}>"
  end

  def python_name
    m = @name.match /(Vector|Map|IterableMap)<(.+)>/
    raise "Invalid container type: #{@name}" unless m
    "#{PYTHON_NAMES_MAP[m[1]]}[#{m[2]}]"
  end

  def storage_name_static(index)
    index = index.call if index.is_a?(Proc)
    "#{@base_type_name}Static<#{@sub_type.cpp_name}, #{index}>"
  end
end

class Ir
  attr_reader :types, :variables, :namespace, :data

  PRIMITIVE_TYPES = %w(u8 i8 u16 i16 u32 i32 u64 i64 u128 i128 u256 i256 bool)

  def initialize(data)
    @types = {}
    @data = data
    @variables = nil
    @namespace = nil
  end

  def load()
    @namespace = 'evm::' + (@data.dig('contract', 'storage_namespace') || 'stor')
    PRIMITIVE_TYPES.each { |type| @types[type] = PrimitiveType.new(type) }

    @data['types'].each { |type| get_or_create_type(type) }

    fields = @data['variables']&.map { |var|
      OpenStruct.new({'name' => var['name'], 'type' => get_or_create_type(var['type']), 'data' => var})
    } || []
    @variables = StructType.new("variables", @data['variables'], fields)
  end

  def get_or_create_type(str)
    str = str['name'] if str.is_a?(Hash)
    existing_type = @types[str]
    return existing_type if existing_type
    m = str.match /(\w+)(\<(\w+)\>)?/
    base_type_name, sub_type = m[1], m[3]
    if sub_type
      raise "Unknown container type" unless %(Vector Map IterableMap).include? base_type_name
      sub_type = get_or_create_type(sub_type)
      @types[str] = ContainerType.new(str, base_type_name, sub_type)
    else
      type_dscr = @data['types'].find { _1['name'] == str }
      raise "Type not found: #{str}" unless type_dscr
      fields = []
      type_dscr['fields'].each do |field|
        type = get_or_create_type(field['type'])
        fields << OpenStruct.new({'name' => field['name'], 'type' => type, 'descr' => field})
      end
      descr = @data['types'].find{ _1['name'] == str }
      @types[str] = StructType.new(str, descr, fields)
    end
  end
end

def canonicalize(data)
  def fix_field(field)
    if field['name']
      raise "type must be specified for field: #{field['name']}" unless field['type']
    else
      field['name'], field['type'] = field.first
    end
  end
  data['types']&.each { _1['fields']&.each { |field| fix_field(field) } }
  data['variables']&.each { fix_field(_1) }
  data['messages']&.each { _1['fields']&.each { |field| fix_field(field) } }
end

def generate
  if options.data_file
    data = YAML.load_file(options.data_file)
  else
    m = File.read(INPUT_FILE).match(/STORAGE_YAML *= *R"\((.*?)\)"/m)
    begin
      data = YAML.load(m ? m[1] : File.read(INPUT_FILE))
    rescue
      return
    end
  end
  data['types'] ||= []
  canonicalize(data)

  ir = Ir.new(data)
  ir.load()

  filename = "#{__dir__}/templates/#{options.generate}.erb"
  t = ERB.new(File.read(filename), nil, '%-')
  t.filename = filename
  res = t.result(binding)
  if options.output
    Pathname(options.output).dirname.mkpath
    File.write(options.output, res)
  else
    puts res
  end
end

def compile
  raise "Path to `ecc` compiler must be specified" unless options.ecc
  clang_cmd = "#{options.ecc} #{INPUT_FILE} -o #{options.output} "
  clang_cmd += ' -v ' if options.verbose
  clang_cmd += ' -std=c++17 '
  clang_cmd += ' -O3 '
  clang_cmd += ' -Xlinker --no-ctor ' if options.no_ctor
  clang_cmd += " -Xlinker #{options.linker_args}" if options.linker_args
  clang_cmd += " #{options.clang_args}" if options.clang_args
  clang_cmd += " -nostdlib" if options.no_stdlib
  clang_cmd += " -save-temps" if options.save_temps
  command(clang_cmd)
end

def main
  generate if options.generate
  compile if options.compile
end

main

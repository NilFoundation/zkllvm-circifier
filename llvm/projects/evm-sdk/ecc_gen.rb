#!/usr/bin/env ruby
# frozen_string_literal: true

require 'erb'
require 'optparse'
require 'ostruct'
require 'yaml'

$options = OpenStruct.new
OptionParser.new do |opts|
  opts.banner = 'Usage: ecc_gen.rb [options] FILE'
  opts.on('--output=PATH', 'Path to output file')
  opts.on('-c', '--cpp-input', 'Input file is a c++ file with embedded storage description') { $options.cpp_input = _1 }
  opts.on('-v', '--verbose', 'Verbose logging')
end.parse!(into: $options)

def options; $options; end

INPUT_FILE=ARGV.pop
raise "Input file must be provided" unless INPUT_FILE

options.output = File.basename(INPUT_FILE, '.*') + '_gen.h' unless options.output

$primitive_types = %w(uint8_t uint16_t uint32_t uint64_t uint128_t uint256_t
                       int8_t int16_t int32_t int64_t int128_t int256_t
                       bool int unsigned)

def to_recursive_ostruct(value)
  if value.is_a?(Array)
    value.each_with_object([]) do |val, memo|
      memo << ((val.is_a?(Hash) || val.is_a?(Array)) ? to_recursive_ostruct(val) : val)
    end
  else
    OpenStruct.new(value.each_with_object({}) do |(key, val), memo|
      memo[key] = (val.is_a?(Hash) || val.is_a?(Array)) ? to_recursive_ostruct(val) : val
    end)
  end
end

def load_input_file(file)
  return File.read(file) unless options.cpp_input
  m = File.read(file).match(/STORAGE_YAML *= *R"\((.*?)\)"/m)
  m ? m[1] : nil
end

def get_cpp_type_static(type, i)
  if $primitive_types.include?(type)
    "SlotStatic<#{type}, Key + #{i}>"
  else
    m = type.match /(\w+)(\<(\w+)\>)?/
    raise "Invalid type: #{type}" unless m
    base_type, sub_type = m[1], m[3]
    sub_type = "#{sub_type}, " if sub_type
    "#{base_type}Static<#{sub_type}Key + #{i}>"
  end
end

def get_cpp_type(type, field_pos)
  if $primitive_types.include?(type)
    "Slot<#{type}>"
  else
    m = type.match /(\w+)(\<(\w+)\>)?/
    raise "Invalid type: #{type}" unless m
    base_type, sub_type = m[1], m[3]
    if sub_type
      "#{base_type}<#{sub_type}>"
    else
      base_type
    end
  end
end

def parse_type(type)
  m = type.match /(\w+)(\<(\w+)\>)?/
  raise "Invalid type: #{var.type}" unless m
  [m[1], m[3]]
end

def calculate_total_size(type, types)
  return type.total_size if type.total_size
  type.total_size = type.fields.reduce(0) { |sum, x|
    base_type, _ = parse_type(x.type)
    raise "Unknown type: #{base_type}" unless types[base_type]
    sum + calculate_total_size(types[base_type], types)
  }
end

def main
  data = load_input_file(INPUT_FILE)
  data = data ? to_recursive_ostruct(YAML.load(data)) : OpenStruct.new
  data.types ||= []

  types = Hash[data.types.map{ [_1.name, _1]}]
  types["Map"] = OpenStruct.new({name: "Map", total_size: 1})
  types["Vector"] = OpenStruct.new({name: "Vector", total_size: 1})
  types["IterableMap"] = OpenStruct.new({name: "IterableMap", total_size: 4})

  $primitive_types.each do |type|
    types[type] = OpenStruct.new({name: type, total_size: 1})
  end

  types.each do |name, type|
    type.total_size = calculate_total_size(type, types)
    if type.fields
      type.fields.each_with_index do |field, i|
        field.cpp_name_static = get_cpp_type_static(field.type, i)
        field.cpp_name = get_cpp_type(field.type, i)
        field.ctor_initializer = "key + #{i}"
      end
      type.ctor_initializer = type.fields.map{ |x| "#{x.name}(#{x.ctor_initializer})" }.join(', ')
    end
  end

  slot = 0
  data.variables&.each_with_index do |var, i|
    var.slot = slot
    if $primitive_types.include?(var.type)
      var.cpp_type = "SlotStatic<#{var.type}, #{slot}>"
      slot += 1
    else
      type, subtype = parse_type(var.type)
      var.cpp_type = "#{type}Static<#{subtype ? subtype + ', ' : ''}#{slot}>"
      raise "Invlid type: #{type.name}" unless types[type].total_size
      slot += types[type].total_size
    end
  end

  t = ERB.new($template, nil, '%-')
  out = t.result(binding)
  if !options.output
    $stdout.write(out)
  else
    File.write(options.output, out)
  end
end

$template = <<EOC
#include "evm_sdk.h"
#include "storage.h"

namespace evm::stor {

% data.types&.reverse.each_with_index do |type, i|
struct <%= type.name %> {
  static constexpr unsigned FIELDS_NUM = <%= type.total_size %>;

  <%= type.name %>(__int256_t key): <%= type.ctor_initializer %> {}
%   type.fields.each do |field|
  <%= field.cpp_name %> <%= field.name %>;
%   end
};

template<__uint256_t Key>
struct <%= type.name %>Static {
  static constexpr unsigned FIELDS_NUM = <%= type.total_size %>;
%   type.fields&.each do |field|
  <%= field.cpp_name_static %> <%= field.name %>;
%   end
};
% end

% data.variables&.each do |var|
<%= var.cpp_type %> <%= var.name %>;
% end

}  // namespace evm::stor
EOC

main

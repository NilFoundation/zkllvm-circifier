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
  raise "STORAGE_YAML is not recognized in C++ file #{file}" unless m
  m[1]
end

def main
  data = to_recursive_ostruct(YAML.load(load_input_file(INPUT_FILE)))

  def primitive_type?(type)
    (type =~ /^(__)?(u?int((8|16|32|64|256)_t)?|unsigned|char|long)$/) == 0
  end

  def get_cpp_type_static(type, i)
    if primitive_type?(type)
      "SlotStatic<#{type}, Key + #{i}>"
    else
      m = type.match /(\w+)(\<(\w+)\>)?/
      raise "Invalid type: #{type}" unless m
      base_type, sub_type = m[1], m[3]
      if sub_type
        raise "Only Dict can be templated type: #{type}" if base_type != "Dict"
        "#{base_type}Static<#{sub_type}, Key + #{i}>"
      else
        "#{base_type}Static<Key + #{i}>"
      end
    end
  end

  def get_cpp_type(type, field_pos)
    if primitive_type?(type)
      "Slot<#{type}>"
    else
      m = type.match /(\w+)(\<(\w+)\>)?/
      raise "Invalid type: #{type}" unless m
      base_type, sub_type = m[1], m[3]
      if sub_type
        raise "Only Dict can be templated type: #{type}" if base_type != "Dict"
        "#{base_type}<#{sub_type}, #{field_pos}>"
      else
        base_type
      end
    end
  end

  data.variables.each_with_index do |var, i|
    var.slot = i
    if primitive_type?(var.type)
      var.cpp_type = "SlotStatic<#{var.type}, #{i}>"
    else
      m = var.type.match /(\w+)(\<(\w+)\>)?/
      raise "Invalid type: #{var.type}" unless m
      type, subtype = m[1], m[3]
      var.cpp_type = "#{type}Static<#{subtype ? subtype + ', ' : ''}#{i}>"
    end
  end

  data.types.each do |type|
    type.fields.each_with_index do |field, i|
      field.cpp_name_static = get_cpp_type_static(field.type, i)
      field.cpp_name = get_cpp_type(field.type, i)
      field.ctor_initializer = field.type.start_with?("Dict<") ? "key" : "key + #{i}"
    end
    type.ctor_initializer = type.fields.map{ |x| "#{x.name}(#{x.ctor_initializer})" }.join(', ')
  end

  t = ERB.new($template, nil, '%-')
  out = t.result(binding)
  if !options.output
    $stdout.write(out)
  else
    File.open(options.output, 'w') { _1.write(out) }
  end
end

$template = <<EOC
#include "evm_sdk.h"

namespace evm::stor {

% data.types.reverse.each_with_index do |type, i|
struct <%= type.name %> {
  <%= type.name %>(__int256_t key): <%= type.ctor_initializer %> {}
%   type.fields.each do |field|
  <%= field.cpp_name %> <%= field.name %>;
%   end
};

template<__uint256_t Key>
struct <%= type.name %>Static {
%   type.fields.each do |field|
  <%= field.cpp_name_static %> <%= field.name %>;
%   end
};
% end

% data.variables.each_with_index do |var, i|
<%= var.cpp_type %> <%= var.name %>;
% end

}  // namespace evm::stor
EOC

main

require_relative 'dsl'
source = {}
File.open(ARGV[0], "r") do |file| 
  source = file.read
end

dsl_instance = DSL.new 
dsl_instance.instance_eval(source)

dsl_instance.dump_buffer_to_file(ARGV[1])
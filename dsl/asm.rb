require_relative 'dsl_wrapper'
include DSLWrapper
dsl_obj = dsl do
  add x1, x2, x3
  add x1, x2, x3
end

dsl_obj.dump_buffer_to_file("output.bin")
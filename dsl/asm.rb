require_relative 'dsl_wrapper'
include DSLWrapper

dsl_obj = dsl do
  j 12
  add x1, x2, x3
  movn x1, x2, x3
  rbit x1, x2
  slti x1, x3, 64
  cbit x1, x2, 16
  bne x1, x2, 2
  bdep x1, x2, x3
  beq x1, x2, 42
  usat x1, x2, 1
  ld x1, 32.(x2)
  st x1, 32.(x2)

  stp x1, x2, 32.(x3)
  j gay
  label gay
  syscall
  # Halls of awaiting:
  # syscall exit
end

dsl_obj.dump_buffer_to_file("output.bin")

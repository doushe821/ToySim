require_relative 'dsl'
local_dsl = DSL.Initialize

Proc.new(assemble do ||
  ADD x1, x2, x3
  ADD x1, x2, x3
end)

local_dsl.func(assemble)

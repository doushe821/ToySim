require_relative 'dsl'

module DSLWrapper
  def dsl(&block) # whats &?
    dsl_instance = DSL.new
    dsl_instance.instance_eval(&block)
    dsl_instance
  end
end
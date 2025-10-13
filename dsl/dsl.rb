# TODO rip & tear
# find 26
module Kernel 
  undef :syscall
end
class DSL
  REGISTER_PATTERN = /\A[xif][0-9]+\z/
  LABEL_NAME_PATTERN = /\A[a-zA-Z_][a-zA-Z0-9_]*\z/
  REG_MAP = {
    'x0' => 0, 'x1' => 1, 'x2' => 2, 'x3' => 3, 'x4' => 4, 'x5' => 5, 'x6' => 6, 'x7' => 7,
    'x8' => 8, 'x9' => 9, 'x10' => 10, 'x11' => 11, 'x12' => 12, 'x13' => 13, 'x14' => 14, 'x15' => 15,
    'x16' => 16, 'x17' => 17, 'x18' => 18, 'x19' => 19, 'x20' => 20, 'x21' => 21, 'x22' => 22, 'x23' => 23,
    'x24' => 24, 'x25' => 25, 'x26' => 26, 'x27' => 27, 'x28' => 28, 'x29' => 29, 'x30' => 30, 'x31' => 31,
  }.freeze

  OPCODES = {
    'add' => 0b011000, 'j' => 0b111001, 'movn' => 0b111011, 'rbit' => 0b010110,
    'slti' => 0b110101, 'ld' => 0b010010, 'syscall' => 0b011001, 'cbit' => 0b111110,
    'stp' => 0b101010, 'bne' => 0b110111, 'usat' => 0b100011, 'beq' => 0b001011,
    'bdep' => 0b001100, 'st' => 0b100101
  }.freeze

  INSTRUCTION_LAYOUTS = {
    add: {
      opcode: OPCODES['add'],
      funct5: 0b00000,
      funct6: 0b000000,
      fields: [
        [:funct6, 6, 26],
        [:rs,    5, 21],
        [:rt,    5, 16],
        [:rd,     5, 11],
        [:funct5, 5, 6],
        [:opcode, 6, 0 ]
      ],
      order: [0, 2, 1]
    },
    j: {
      opcode: OPCODES['j'],
      fields: [
        [:opcode, 6, 26],
        [:index, 26, 0]
      ], 
      order: [0]
    },
    movn: {
      opcode: OPCODES['movn'],
      funct5: 0b00000,
      funct6: 0b000000,
      fields: [
        [:funct6, 6, 26],
        [:rs, 5, 21],
        [:rt, 5, 16],
        [:rd, 5, 11],
        [:funct5, 5, 6],
        [:opcode, 6, 0]
      ], 
      order: [0, 2, 1]
    },
    rbit: {
      opcode: OPCODES['rbit'],
      funct10: 0b0000000000,
      funct6: 0b000000,
      fields:[
        [:funct6, 6, 26],
        [:rd, 5, 21],
        [:rs, 5, 16],
        [:funct10, 10, 6],
        [:opcode, 6, 0]
      ],
      order: [1, 0]
    },
    slti: {
      opcode: OPCODES['slti'],
      funct6: 0b110101,
      fields: [
        [:opcode, 6, 26],
        [:rs, 5, 21],
        [:rt, 5, 16],
        [:imm, 16, 0]
      ], 
      order: [2, 0, 1]
    },
    ld: {
      opcode: OPCODES['ld'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ],
      order: [2, 0, 1]
    },
    syscall: {
      opcode: OPCODES['syscall'],
      funct6: 0b000000,
      funct20: 0b00000000000000000000,
      fields: [
        [:funct6, 6, 26],
        [:funct20, 20, 6],
        [:opcode, 6, 0]
      ], 
      order: [] # Might die
    },
    cbit: {
      opcode: OPCODES['cbit'],
      funct11: 0b00000000000,
      fields: [
        [:opcode, 6, 26],
        [:rd, 5, 21],
        [:rs, 5, 16],
        [:imm, 5, 11],
        [:funct11, 11, 0]
      ],
      order: [2, 1, 0]
    },
    stp: {
      opcode: OPCODES['stp'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt1, 5, 16],
        [:rt2, 5, 11],
        [:offset, 11, 0]
      ],
      order: [3, 1, 0, 2]
    },
    bne: {
      opcode: OPCODES['bne'],
      fields: [
        [:opcode, 6, 26], 
        [:rs, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ],
      order: [2, 1, 0]
    },
    usat: {
      opcode: OPCODES['usat'],
      funct11: 0b00000000000,
      fields: [
        [:opcode, 6, 26],
        [:rd, 5, 21],
        [:rs, 5, 16],
        [:imm, 5, 11],
        [:funct11, 11, 0]
      ],
      order: [2, 1, 0]
    },
    beq: {
      opcode: OPCODES['beq'],
      fields: [
        [:opcode, 6, 26],
        [:rs, 5, 21],
        [:rd, 5, 16],
        [:offset, 16, 0]
      ],
      order: [2, 1, 0]
    },
    bdep: {
      opcode: OPCODES['bdep'],
      funct6: 0b000000,
      funct5: 0b00000,
      fields: [
        [:funct6, 6, 26],
        [:rd, 5, 21],
        [:rs1, 5, 16],
        [:rs2, 5, 11],
        [:funct5, 5, 6],
        [:opcode, 6, 0]
      ],
      order: [2, 1, 0]
    },
    st: {
      opcode: OPCODES['st'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ],
      order: [2, 0, 1]
    },
    label: {
    }
  }.freeze


  def initialize
    @buffer = []
    @PC = 0
    @labels = {}
  end

  def method_missing(method_name, *args, &block)
    if method_name.to_s.match?(REGISTER_PATTERN)
      if args.any? || block
        super
      else
        method_name
      end
    elsif OPCODES.key?(method_name.to_s)
      handle_instruction_call(method_name, *args)
    elsif !args.any? && method_name.to_s.match?(LABEL_NAME_PATTERN) # That's a label
      method_name
    else
      raise "Lexer failure, undefined token"
    end
  end

  def respond_to_missing?(method_name, include_private = false)
    method_name.to_s.match?(REGISTER_PATTERN) || INSTRUCTION_LAYOUTS.key?(method_name) || super
  end

  def label(label_name)
    # TODO namecheck maybe
    if @labels.key?(label_name)
      label_info = @labels[label_name]

      if label_info[:address] != nil
        raise "Label redefinition: #{label_name} on PC 0x#{@PC}, previously defined on PC 0x#{label_info[:address]}"
      end

      label_info[:address] = @PC
      label_info[:pending_offsets].each do |instr_pc, offset_size, offset_start| # Back patching
        relative_offset = @PC - instr_pc # TODO
        offset_mask = (1 << offset_size) - 1
        relative_offset = relative_offset & offset_mask
        relative_offset = relative_offset << offset_start
        @buffer[instr_pc / 4] |= relative_offset
        puts "Backpatch completed: #{label_name}: 0x#{@buffer[instr_pc / 4].to_s}"
      end

    else 
      @labels[label_name] = { address: @PC, pending_offsets: [[]]}
      puts "Defined label : #{label_name} at address 0x#{@PC.to_s}"
    end
  end

  private

  def handle_instruction_call(instruction_name, *operands)
    layout = INSTRUCTION_LAYOUTS[instruction_name]
    unless layout
      raise NoMethodError, "Undefined instruction: #{instruction_name}"
    end
    operands = operands.flatten
    puts "Operands flatten: #{operands}"
    operands = layout[:order].map { |index| operands[index] } # operands[i] = operands[order[i]]
    puts "Operands in order: #{operands}"
    operands = operands.reverse
    puts "Operands reversed: #{operands}"
    args_map = {}
    operand_index = 0
    layout[:fields].each do |field_type, size, start_bit|
      case field_type
      when :rd, :rs1, :rs2, :rt, :rt1, :rt2, :rs, :base
        reg_name = operands[operand_index]
        reg_num = REG_MAP[reg_name.to_s]
        if reg_num.nil?
          raise ArgumentError, "Invalid register specified for #{field_type}: #{reg_name}"
        end
        args_map[field_type] = reg_num
        operand_index += 1
      when :imm5, :imm
        imm_value = operands[operand_index]
        # TODO imm validation
        args_map[field_type] = imm_value
        operand_index += 1
      when :index, :offset
        if operands[operand_index].is_a?(Integer) # Jump in Imm
          imm_value = operands[operand_index]
          args_map[field_type] = imm_value
          operand_index += 1
        else # Jump on label
          label_name = operands[operand_index]
          if @labels.key?(label_name)
            label_info = @labels[label_name]
            if label_info[:address] == nil

              @labels[label_name][:pending_offsets] << [@PC, size, start_bit] # TODO learn how to update correctly

              args_map[field_type] = 0
              operand_index += 1
            else 
              label_address = label_info[:address]
              relative_offset = label_address - @PC
              args_map[field_type] = relative_offset
              operand_index += 1
            end
          else
              @labels[label_name] = { address: nil, pending_offsets: [[@PC, size, start_bit]]}
              args_map[field_type] = 0
              operand_index += 1
          end
        end
      when :funct3, :funct5, :funct6, :funct7, :funct10, :funct11, :funct20, :opcode
        args_map[field_type] = layout[field_type]
      else
        raise ArgumentError "Incorrect operand for field type #{field_type}: #{operands[operand_index]}"
      end
    end

    # Encode the instruction based on the layout and arguments
    encoded_instruction = encode_instruction(layout, args_map)

    # Write the encoded instruction to the buffer
    @buffer << encoded_instruction
    puts "Encoded #{instruction_name} (#{operands.join(', ')}): 0x#{encoded_instruction.to_s(16).rjust(8, '0').upcase}"
    @PC += 4
    # Optional: Return the encoded instruction or the DSL object for chaining
    encoded_instruction
  end

  private

  def encode_instruction(layout, args_map)
    instruction = 0

    layout[:fields].each do |field_type, size, start_bit|
      value_to_insert = args_map[field_type]
      if value_to_insert.nil?
        raise ArgumentError "Missing value for field type: #{field_type}"
      end
      # Mask the value to fit within the specified size
      mask = (1 << size) - 1
      masked_value = value_to_insert & mask

      # Shift to the correct position
      shifted_value = masked_value << start_bit

      # Add value to instrucition
      instruction |= shifted_value
    end

    instruction
  end

  public

  def dump_buffer_to_file(filename)
    File.open(filename, 'wb') do |file|
      @buffer.each do |instruction_int|
        file.write [instruction_int].pack('V') # V is for lil endian
      end
    end
    puts "Buffer dumped to #{filename}. Buffer size: #{@buffer.length} instructions."
  end
end

class Integer
  def call(reg)
    return [reg, self]
  end
end


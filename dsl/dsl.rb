# TODO support labels and offset.(base) syntax


class DSL

  REGISTER_PATTERN = /\A[xif][0-9]+\z/
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
  # Define the instruction layout table
  # Key: Instruction symbol (e.g., :add)
  # Value: Hash containing :opcode, :fields (array of [type, size_in_bits, start_bit_position])
  # Note: Bit positions are typically shown with bit 0 as LSB. Here we define [start_bit, size]
  # so e.g., [7, 5] means 5 bits starting from bit 7 (occupying bits 7-11).
  INSTRUCTION_LAYOUTS = {
    add: {
      opcode: OPCODES['add'], # R-type opcode
      funct5: 0b00000,     # Specific to ADD
      funct6: 0b000000, # Specific to ADD
      fields: [
        # [field_type, size_in_bits, start_bit_position]
        [:funct6, 6, 26], # funct7 (7 bits) starts at bit 25
        [:rs,    5, 21], # rs2 (5 bits) starts at bit 20
        [:rt,    5, 16], # rs1 (5 bits) starts at bit 15
        [:rd,     5, 11], # rd (5 bits) starts at bit 7
        [:funct5, 5, 6], # funct3 (3 bits) starts at bit 12
        [:opcode, 6, 0 ]  # opcode (7 bits) starts at bit 0
      ]
    },
    j: {
      opcode: OPCODES['j'],
      fields: [
        [:opcode, 6, 26],
        [:index, 26, 0]
      ]
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
      ]
    },
    rbit: {
      opcode: OPCODES['rbit'],
      funct10: 0b0000000000,
      funct6: 0b000000,
      fields:[
        [:funct6, 6, 26],
        [:rs, 5, 21],
        [:rs, 5, 16],
        [:funct10, 10, 6],
        [:opcode, 6, 0]
      ]
    },
    slti: {
      opcode: OPCODES['slti'],
      funct6: 0b110101,
      fields: [
        [:opcode, 6, 26],
        [:rs, 5, 21],
        [:rt, 5, 16],
        [:imm, 16, 0]
      ]
    },
    ld: {
      opcode: OPCODES['ld'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ]
    },
    syscall: {
      opcode: OPCODES['syscall'],
      funct6: 0b000000,
      fields: [
        [:funct6, 6, 26],
        [:code, 20, 6],
        [:opcode, 6, 0]
      ]
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
      ]
    },
    stp: {
      opcode: OPCODES['stp'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt1, 5, 16],
        [:rt2, 5, 11],
        [:offset, 11, 0]
      ]
    },
    bne: {
      opcode: OPCODES['bne'],
      fields: [
        [:opcode, 6, 26], 
        [:rs, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ]
    },
    usat: {
      opcode: OPCODES['usat'],
      funct11: 0b00000000000,
      fields: [
        [:opcode, 6, 26],
        [:rd, 5, 21],
        [:rs, 5, 16],
        [:imm, 5, 11],
        [:funct11, 0]
      ]
    },
    beq: {
      opcode: OPCODES['beq'],
      fileds: [
        [:opcode, 6, 26],
        [:rs, 5, 21],
        [:rd, 5, 16],
        [:offset, 16, 0]
      ]
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
      ]
    },
    st: {
      opcode: OPCODES['st'],
      fields: [
        [:opcode, 6, 26],
        [:base, 5, 21],
        [:rt, 5, 16],
        [:offset, 16, 0]
      ]
    }
  }.freeze

  def method_missing(method_name, *args, &block)
    if method_name.to_s.match?(REGISTER_PATTERN)
      if args.any? || block
        super
      else
        method_name
      end
    else
      # super
      handle_instruction_call(method_name, *args)
    end
  end

  def respond_to_missing?(method_name, include_private = false)
    method_name.to_s.match?(REGISTER_PATTERN) || INSTRUCTION_LAYOUTS.key?(method_name) || super
  end

  def initialize
    @buffer = []
    # @buffer = String.new.force_encoding('ASCII-8BIT')
    @IP = 0
    @labels = {}
  end


  private 

  def handle_instruction_call(instruction_name, *operands)
    layout = INSTRUCTION_LAYOUTS[instruction_name]
    unless layout
      raise NoMethodError, "Undefined instruction: #{instruction_name}"
    end

  # Prepare arguments map based on field types and provided operands
    args_map = {}
    operand_index = 0
    layout[:fields].each do |field_type, size, start_bit|
      case field_type
      when :rd, :rs, :rs1, :rs2, :rt, :base
        reg_name = operands[operand_index]
        reg_num = REG_MAP[reg_name.to_s]
        if reg_num.nil?
          raise ArgumentError, "Invalid register specified for #{field_type}: #{reg_name}"
        end
        args_map[field_type] = reg_num
        operand_index += 1
      when :imm, :offset
        imm_value = operands[operand_index]
        # Optional: Add immediate value validation/range checking here
        # e.g., for 12-bit signed immediate: -2048 to 2047
        # For simplicity here, just pass the value
        args_map[field_type] = imm_value
        operand_index += 1
      when :funct3, :funct5, :funct6, :funct7, :funct10, :funct11, :opcode
        # These are usually fixed values defined in the layout
        args_map[field_type] = layout[field_type]
      else
        # Handle other fixed fields if present (e.g., specific bits for CSR instructions)
        # For now, assume they are defined in the layout or passed as operands if needed
        # You might need a more complex mapping for non-register, non-fixed fields
        # If a field type is expected from operands and not handled above:
        # args_map[field_type] = operands[operand_index]
        # operand_index += 1
      end
    end

    # Encode the instruction based on the layout and arguments
    encoded_instruction = encode_instruction(layout, args_map)

    # Write the encoded instruction to the buffer
    @buffer << encoded_instruction
    puts "Encoded #{instruction_name} (#{operands.join(', ')}): 0x#{encoded_instruction.to_s(16).rjust(8, '0').upcase}"

    # Optional: Return the encoded instruction or the DSL object for chaining
    encoded_instruction
  end

  private 

  def encode_instruction(layout, args_map)
    instruction = 0

    layout[:fields].each do |field_type, size, start_bit|
      value_to_insert = args_map[field_type]
      if value_to_insert.nil?
        raise "Missing value for field type: #{field_type}"
      end

      # Mask the value to fit within the specified size
      mask = (1 << size) - 1
      masked_value = value_to_insert & mask

      # Shift the masked value to its correct position
      shifted_value = masked_value << start_bit

      # Combine it with the instruction using OR
      instruction |= shifted_value
    end
    @IP += 1
    instruction
  end

  public
  def dump_buffer_to_file(filename)
    File.open(filename, 'wb') do |file|
      @buffer.each do |instruction_int|
        file.write [instruction_int].pack('V') # Use 'N' if big-endian is required
      end
    end
    puts "Buffer dumped to #{filename}. Buffer size: #{@buffer.length} instructions."
  end
end

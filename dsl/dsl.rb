# TODO good offset encoding
# TODO magic numbe@registers[rs] replacement
# TODO main assembly pass function
# TODO more verification maybe


class DSL

  def self.Initialize
    @buffer = String.new.force_encoding('ASCII-8BIT')
    @IP = 0
    @labels = {}
    @registers = (0..31).each_with_object({}) do |i, hash|
      hash["x#{i}"] = i
    end
    return self
  end

  def self.ADD(rd, rs, rt)
    @buffer << [0b011000 + (@registers[rd] << 10) + (@registers[rt] << 15) + (@registers[rs] << 20)].pack('L<')
    ++@IP;
  end


  def self.patchJumps(label, jump_positions, label_position)
    jump_positions.each do |jump_position|
      jump_imm = label_position - jump_position + 4 # GAY
      patch = [(0b111001 << 25) + jump_imm].pack('L<')  
      @buffer[jump_position, 4] = patch # GAY
    end
    @labels[label][1] = []
  end

  def self.verifyLabels
    @labels.each do |label, (label_position, jump_positions)|
      if label_position.nil? && jump_positions.any?
        raise "Undefined label: #{label} referenced by #{jumps.size} jumps on positions: #{jump_positions.join{', '}}"
      end
    end
  end

  def self.l(label) # think
    if @labels.key?(label)
      label_data = @labels[label]

      label_data[0] = @IP

      jump_positions = label_data[1]

      patchJumps(label, jump_positions, @IP)
    else
      @labels[label] = [@IP, []]
    end
  end

  def self.J(target) # think
    if target.is_a?(Integer)
      @buffer << [(0b111001 << 25) + target].pack('L<')
    elsif target.is_a?(String)
      if @labels.key?(target)
        label_position =  labels[target][0]
        jump_imm = label_position - @IP + 4 # GAY
        @buffer << ((0b111001 << 25) + jump_imm)
      else
        @labels[target]=[nil,[@IP]]
      end
    else
      raise "Unsupported type of J argument: it should be an integer or a label: @IP = #{@IP}"
    end
    ++@IP;
  end

  def self.MOVN(rd, rs, rt)
    @buffer << ((0b111011) + (@registers[rd] << 10) + (@registers[rt] << 15) + (@registers[rs] << 20)).pack('L<')
    ++@IP;
  end

  def self.RBIT(rd, rs)
    @buffer << (0b010110 + (@registers[rs] << 15) + (@registers[rd] << 20)).pack('L<')
    ++@IP;
  end

  def self.SLTI(rt, rs, imm)
    @buffer << (imm + (@registers[rt] << 15) + (@registers[rs] << 20) + (0b110101 << 25)).pack('L<')
    ++@IP;
  end

  def self.LD(rt) # think offset(base)
    ++@IP;
  end

  def self.SYSCALL() # think
    ++@IP;
  end

  def self.CBIT(rd, rs, imm5)
    @buffer << ((imm5 << 10) + (@registers[rs] << 15) + (@registers[rd] << 20) + (0b111110 << 25)).pack('L<')
    ++@IP;
  end

  def self.STP(rt1, rt2) # think offset(base)
    # @buffer << ().pack('L<')
    ++@IP;
  end

  def self.BNE(rs, rt, offset)
    @buffer << (offset + (@registers[rt] << 15) + (@registers[rs] << 20) + (0b110111 << 25)).pack('L<')
    ++@IP;
  end
  def self.USAT(rd, rs, imm5)
    @buffer << ((imm5 << 10) + (@registers[rs] << 15) + (@registers[rd] << 20) + (0b100011 << 25)).pack('L<')
    ++@IP;
  end

  def self.BEQ(rs, rt, offset)
    @buffer << (offset + (@registers[rt] << 15) + (@registers[rs] << 20) + (0b001011 << 25)).pack('L<')  
    ++@IP;
  end

  def self.BDEP(rd, rs1, rs2)
    @buffer << (0b001100 + (@registers[rs2] << 10) + (@registers[rs1] << 15) + (@registers[rd] << 20)).pack('L<')
    ++@IP;
  end

  def self.ST(rt) # think offset(base)
    @buffer << ().pack('L<')
    ++@IP;
  end

  def self.dump
    File.open('output.bin', 'wb') do |file|
      file.write(@buffer)
    end
  end
end


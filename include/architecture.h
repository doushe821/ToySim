#pragma once
#include <cstdint>
#include <vector>
namespace ToySim {

enum LocalOpcodes {
  J,
  MOVN,
  RBIT,
  ADD,
  SLTI,
  LD,
  SYSCALL,
  CBIT,
  STP,
  BNE,
  USAT,
  BEQ,
  BDEP,
  ST,
  UNDEFINED
};

enum InstructionTypesCodes {
  JType, // OpCode, imm
  BType, // OpCode, rs, rt, offset / OpCode, base, rt, offset
  CType, // 0, rs, rt, rd, 0, OpCode
  CDType, // 0, rd, rs, 0, OpCode
  SType, // 0, code, OpCode
  Dude, // OpCode, rd, rs, imm5, 0
  Wat, // OpCode, base, rt1, rt2, offset
  ImOut, // 0, rd, rs1, rs2, 0, OpCode,
  Store, // OpCode, base, rt, offset
};

enum EncodingPartCode {
  OpCodeEncodingPart,
  ImmEncodingPart,
  Reg,
};

using EncodingPart = std::pair<EncodingPartCode, unsigned>;

struct InstructionType {
  
};


enum OpCodes {
  OpCodeJ = 0b111001,
  OpCodeMOVN = 0b111011,
  OpCodeRBIT = 0b010110,
  OpCodeADD = 0b110000,
  OpCodeSLTI = 0b110101,
  OpCodeLD = 0b010010,
  OpCodeSYSCALL = 0b011001,
  OpCodeCBIT = 0b111110,
  OpCodeSTP = 0b101010,
  OpCodeBNE = 0b110111,
  OpCodeUSAT = 0b100011,
  OpCodeBEQ = 0b001011,
  OpCodeBDEP = 0b001100,
  OpCodeST = 0b100101,
};

struct Operand { // TODO support uneven encodings
  unsigned Size;
  unsigned StartingBit;
};

struct Instruction {
  char OpCode;
  enum LocalOpcodes LocalOpcode = UNDEFINED;
  InstructionTypes InstructionType;
  unsigned OperandNum;
  std::vector<int> Operands;
};

class SPU {
private:
  static const unsigned RegNum = 32;
  static const unsigned MemorySize = 1024;
  char Memory[MemorySize];
  int Regs[RegNum];

public:
  void Compute(char* Filename);
  void StateDump() const;
  void RegDump() const;
  void MemoryDump() const;
};

} // namespace ToySim

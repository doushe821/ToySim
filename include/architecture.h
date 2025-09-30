#pragma once
#include <vector>
#include <cassert>
namespace ToySim {

enum InstructionTypesCodes {
  JType, // OpCode, imm
  BType, // OpCode, rs, rt, offset / OpCode, base, rt, offset
  EType, // OpCode, rd, rs, imm5, 0
  DSType, // OpCode, base, rt1, rt2, offset
  SType, // OpCode, base, rt, offset
  CType, // 0, rs, rt, rd, 0, OpCode
  CDType, // 0, rd, rs, 0, OpCode
  SysType, // 0, code, OpCode
};

enum EncodingPartCode {
  OpCodeEncoding,
  ImmEncoding,
  RegEncoding,
  ZeroEncoding
};

class EncodingPart {
  EncodingPartCode PartCode;
  unsigned PartSize;
public:
  EncodingPart(EncodingPartCode Code, unsigned Size = 6) { // Optional may be
    PartCode = Code;
    switch (Code) {
      case OpCodeEncoding: {
        PartSize = 6;
        break;
      }
      case ImmEncoding: {
        PartSize = Size;
        break;
      }
      case RegEncoding: {
        PartSize = 5;
        break;
      }
      case ZeroEncoding: {
        PartSize = Size;
        break;
      }
      default: {
        assert(0 && "Wrong part code\n");
      }
    }
  }
};

struct InstructionType {
  enum InstructionTypesCodes Code;
  std::vector<EncodingPart> Layout;
};

static const InstructionType InstructionTypes[] {
  {JType, {{OpCodeEncoding}, {ImmEncoding, 26}}},
  {BType, {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
  {EType, {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 5}, {ZeroEncoding, 11}}},
  {DSType, {{OpCodeEncoding, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 11}}}},
  {SType, {{OpCodeEncoding, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}}},
  {CType, {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 5}, {OpCodeEncoding}}},
  {CDType, {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 10}, {OpCodeEncoding}}},
  {SysType, {{ZeroEncoding}, {ImmEncoding, 20}, {OpCodeEncoding}}}
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

struct Instruction {
  char OpCode;
  InstructionType Type;
};

static const Instruction Instructions[] {
  {OpCodeJ, InstructionTypes[JType]},
  {OpCodeMOVN, InstructionTypes[CType]},
  {OpCodeRBIT, InstructionTypes[CDType]},
  {OpCodeADD, InstructionTypes[CType]},
  {OpCodeSLTI, InstructionTypes[BType]},
  {OpCodeLD, InstructionTypes[BType]},
  {OpCodeSYSCALL, InstructionTypes[SysType]},
  {OpCodeCBIT, InstructionTypes[EType]}, 
  {OpCodeSTP, InstructionTypes[DSType]},
  {OpCodeBNE, InstructionTypes[BType]},
  {OpCodeUSAT, InstructionTypes[EType]},
  {OpCodeBEQ, InstructionTypes[BType]},
  {OpCodeBDEP, InstructionTypes[CType]},
  {OpCodeST, InstructionTypes[BType]}
};

class SPU {
private:
  static const unsigned RegNum = 32;
  static const unsigned MemorySize = 1024;
  char Memory[MemorySize];
  int Regs[RegNum];
  unsigned PC = 0;

public:
  void Compute(std::vector<int> &Instructions);
  void StateDump() const;
  void RegDump() const;
  void MemoryDump() const;
};

} // namespace ToySim

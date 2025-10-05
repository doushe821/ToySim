#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <fstream>
#include <limits>
namespace ToySim {
// Gay

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
public:
  EncodingPartCode PartCode;
  unsigned PartSize;
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
        assert(!"Wrong part code\n");
      }
    }
  }
};

struct InstructionType {
  enum InstructionTypesCodes Code;
  std::vector<EncodingPart> Layout;
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

// Valid.

const unsigned Low6bitMask = 63;
const unsigned High6bitMask = 0xfc000000;

enum OperandTypes {
  RegOT,
  ImmOT
};
struct Operand {
  int Value;
  EncodingPartCode OperandType;
  unsigned Size;
};

const std::unordered_map<const InstructionTypesCodes, const std::vector<EncodingPart>> InstructionTypes { // TODO change name
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
struct Instruction {
  OpCodes OpCode;
  std::vector<Operand> Operands = {};
};


static const std::unordered_map<const OpCodes, std::pair<InstructionTypesCodes, const std::vector<EncodingPart>>> Layouts = {
{OpCodeJ, {JType, InstructionTypes.at(JType)}},
{OpCodeMOVN, {CType, InstructionTypes.at(CType)}},
{OpCodeRBIT, {CDType, InstructionTypes.at(CDType)}},
{OpCodeADD, {CType, InstructionTypes.at(CType)}},
{OpCodeSLTI, {BType, InstructionTypes.at(BType)}},
{OpCodeLD, {BType, InstructionTypes.at(BType)}},
{OpCodeSYSCALL, {SysType, InstructionTypes.at(SysType)}},
{OpCodeCBIT, {EType, InstructionTypes.at(EType)}}, 
{OpCodeSTP, {DSType, InstructionTypes.at(DSType)}},
{OpCodeBNE, {BType, InstructionTypes.at(BType)}},
{OpCodeUSAT, {EType, InstructionTypes.at(EType)}},
{OpCodeBEQ, {BType, InstructionTypes.at(BType)}},
{OpCodeBDEP, {CType, InstructionTypes.at(CType)}},
{OpCodeST, {BType, InstructionTypes.at(BType)}}
};

class SPU {
private:
  // Valid.
  std::vector<int> BinInstructions;
  const unsigned RegNum = 32;
  const unsigned MemorySize = 1024;
  std::vector<int> Memory;
  std::vector<int> Regs;
  unsigned PC = 0;
  // architecture described operations:
  int Reverse(int Val);
  int signExtend(int Val, int N);
  int saturateUnsigned(unsigned Val, unsigned N);

  // Gay

public:
  // Valid.
  SPU(const std::string& FileName, unsigned RegNum = 32, unsigned MemorySize = 1024) : RegNum(RegNum), MemorySize(MemorySize) {
    std::ifstream File(FileName, std::ios::binary);
    if (!File.is_open()) {
        assert(0);
    }
    
    int Instruction;
    while (File >> Instruction) {
      BinInstructions.push_back(Instruction);
    }
    File.close();

    if (RegNum == 0 || MemorySize == 0) {
      assert(0);
    }

    Memory.resize(MemorySize);
    Regs.resize(RegNum);
  }

  Instruction Decode(int Instruction) const;

  // Gay
  void Compute();
  void StateDump() const;
  void RegDump() const;
  void MemoryDump() const;
};

} // namespace ToySim

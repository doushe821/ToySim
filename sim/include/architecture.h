#pragma once
#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <array>
#include <fstream>
#include <iostream>
namespace ToySim {

// architecture defined constants:
static const unsigned kToyISARegNum = 32;

// architecture dependant operations:
// TODO for N-bit
static int reverse(int Val) {
  Val = (Val & 0xFFFF0000) >> 16 | (Val & 0x0000FFFF) << 16;
  Val = (Val & 0xFF00FF00) >> 8  | (Val & 0x00FF00FF) << 8 ;
  Val = (Val & 0xF0F0F0F0) >> 4  | (Val & 0x0F0F0F0F) << 4 ;
  Val = (Val & 0xCCCCCCCC) >> 2  | (Val & 0x33333333) << 2 ;
  Val = (Val & 0xAAAAAAAA) >> 1  | (Val & 0x55555555) << 1 ;
  return Val;
}

static int signExtend(int Val, int N) {
  if (Val & (1 << (N - 1))) {
    int Mask = ~((1 << N) - 1);
    Val |= Mask;
  }
  return Val;
}
static int saturateUnsigned(unsigned Val, unsigned N) {
  unsigned Limit = (1 << N) - 1;
  if (Val > Limit) {
    Val = Limit;
  }
  return Val;
}

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


const unsigned Low6bitMask = 0x3f;
const unsigned High6bitMask = 0xfc000000;

enum OperandTypes {
  RegOT,
  ImmOT
};
struct Operand {
  uint32_t Value;
  EncodingPartCode OperandType;
  unsigned Size;
};

const std::unordered_map<const InstructionTypesCodes, const std::vector<EncodingPart>> InstructionTypes { // TODO change name
  {JType,   {{OpCodeEncoding}, {ImmEncoding, 26}}},
  {BType,   {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
  {EType,   {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 5}, {ZeroEncoding, 11}}},
  {DSType,  {{OpCodeEncoding, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 11}}}},
  {SType,   {{OpCodeEncoding, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}}},
  {CType,   {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 5}, {OpCodeEncoding}}},
  {CDType,  {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 10}, {OpCodeEncoding}}},
  {SysType, {{ZeroEncoding}, {ImmEncoding, 20}, {OpCodeEncoding}}}
};

enum SyscallCodeTable {
  EXIT,
};

enum OpCodes {
  OpCodeJ       = 0b111001,
  OpCodeMOVN    = 0b111011,
  OpCodeRBIT    = 0b010110,
  OpCodeADD     = 0b011000,
  OpCodeSLTI    = 0b110101,
  OpCodeLD      = 0b010010,
  OpCodeSYSCALL = 0b011001,
  OpCodeCBIT    = 0b111110,
  OpCodeSTP     = 0b101010,
  OpCodeBNE     = 0b110111,
  OpCodeUSAT    = 0b100011,
  OpCodeBEQ     = 0b001011,
  OpCodeBDEP    = 0b001100,
  OpCodeST      = 0b100101,
};
struct Instruction {
  OpCodes OpCode;
  std::vector<Operand> Operands = {};
};

// FIXME 
static const std::unordered_map<OpCodes,  const std::vector<EncodingPart>> Layouts = { // FIXME key is always constant
{OpCodeJ,       {{OpCodeEncoding}, {ImmEncoding, 26}}},
{OpCodeMOVN,    {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 5}, {OpCodeEncoding}}},
{OpCodeRBIT,    {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 10}, {OpCodeEncoding}}},
{OpCodeADD,     {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 5}, {OpCodeEncoding}}},
{OpCodeSLTI,    {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
{OpCodeLD,      {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
{OpCodeSYSCALL, {{ZeroEncoding}, {ImmEncoding, 20}, {OpCodeEncoding}}},
{OpCodeCBIT,    {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 5}, {ZeroEncoding, 11}}}, 
{OpCodeSTP,     {{OpCodeEncoding, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 11}}}},
{OpCodeBNE,     {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
{OpCodeUSAT,   {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 5}, {ZeroEncoding, 11}}},
{OpCodeBEQ,    {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}},
{OpCodeBDEP,   {{ZeroEncoding}, {RegEncoding}, {RegEncoding}, {RegEncoding}, {ZeroEncoding, 5}, {OpCodeEncoding}}},
{OpCodeST,     {{OpCodeEncoding}, {RegEncoding}, {RegEncoding}, {ImmEncoding, 16}}}
};

static const unsigned OpCodeMax = 64;

static void HandleInvalidOpCode(ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
  std::cout << "Invalid OpCode on PC = " << PC << ", skipping instruction\n";
}

// TO BE GENERATED
using SyscallHandler = void(*)(std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, unsigned &PC);
constexpr static std::array<SyscallHandler, 1> SyscallTable = {[](std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, unsigned &PC) { std::cout << "\033[1;32mExited with code " << Regs[0] << "\033[37m\n"; PC = UINT32_MAX - 4; }}; // FIXME finished

using InstructionHandler = void(*)(ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC); // SPU as argument
constexpr static std::array<InstructionHandler, OpCodeMax> initInstructionTable() {
  // TODO separate class, inherit from std::array
  std::array<InstructionHandler, OpCodeMax> TempTable;
  std::fill(TempTable.begin(), TempTable.end(), &HandleInvalidOpCode);
// TODO memory as separate structure
  TempTable[OpCodeJ] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 1);
    assert(Ops[0].OperandType == ImmEncoding);
    PC += Ops[0].Value;
  };
  TempTable[OpCodeMOVN] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == RegEncoding);
    auto &rd = Regs[DecodedInstruction.Operands[0].Value];
    assert(Ops[1].OperandType == RegEncoding);
    auto &rt = Regs[DecodedInstruction.Operands[1].Value];
    assert(Ops[2].OperandType == RegEncoding);
    auto &rs = Regs[DecodedInstruction.Operands[2].Value];
    if (rt != 0) {
      rd = rs;
    }
    PC += 4;
  };
  TempTable[OpCodeRBIT] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 2);
    assert(Ops[0].OperandType == RegEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    auto &rs = Regs[Ops[0].Value];
    auto &rd = Regs[Ops[1].Value];
    auto ReversedReg = reverse(rs);
    rd = ReversedReg;
    PC += 4;
  };
  TempTable[OpCodeADD] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == RegEncoding);
    assert(Ops[1].OperandType == RegEncoding);>
    assert(Ops[2].OperandType == RegEncoding);
    auto &rd = Regs[Ops[0].Value];
    auto &rt = Regs[Ops[1].Value];
    auto &rs = Regs[Ops[2].Value];
    rd = rs + rt;
    PC += 4;
  };
  TempTable[OpCodeSLTI] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rt = Regs[Ops[1].Value];
    auto &rs = Regs[Ops[2].Value];
    rt = (rs < signExtend(imm, Ops[0].Size));
    PC += 4;
  };
  TempTable[OpCodeLD] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rt = Regs[Ops[1].Value];
    auto &base = Regs[Ops[2].Value];
    // std::cout << "Base reg = " << Ops[2].Value << '\n';
    // std::cout << "Base = " << base << '\n';
    rt = Memory[base + signExtend(imm, Ops[0].Size)/4];
    PC += 4;
  };
  TempTable[OpCodeSYSCALL] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    // std::cout << "sys has been called\n";
    SyscallTable[Regs[8]](Regs, Memory, PC);
    PC += 4;
  };
  TempTable[OpCodeCBIT] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rs = Regs[Ops[1].Value];
    auto &rd = Regs[Ops[2].Value];
    rd = rs & (~(1 << (imm - 1)));
    PC += 4;
  };
  TempTable[OpCodeSTP] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 4);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    assert(Ops[3].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rt2 = Regs[Ops[1].Value];
    auto &rt1 = Regs[Ops[2].Value];
    auto &base = Regs[Ops[3].Value];
    auto addr = base + signExtend(imm, Ops[0].Size);
    Memory[addr] = rt1;
    Memory[addr + 1] = rt2; // TODO char memory
    PC += 4;
  };
  TempTable[OpCodeBNE] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rt = Regs[Ops[1].Value];
    auto &rs = Regs[Ops[2].Value];
    auto target = signExtend(imm | 0b00, Ops[0].Size);
    auto cond = rs != rt;
    if (cond) {
      PC += target;
    } else {
      PC += 4;
    }
  };
  TempTable[OpCodeUSAT] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rd = Regs[Ops[1].Value];
    auto &rs = Regs[Ops[2].Value];
    rd = saturateUnsigned(rs, imm);
    PC += 4;
  };
  TempTable[OpCodeBEQ] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;
    auto &rt = Regs[Ops[1].Value];
    auto &rs = Regs[Ops[2].Value];
    auto target = signExtend(imm | 0b00, Ops[0].Size);
    auto cond = rs == rt;
    if (cond) {
      PC += target;
    } else {
      PC += 4;
    }
  };
  TempTable[OpCodeBDEP] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == RegEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &rs2 = Ops[0].Value;
    auto &rs1 = Regs[Ops[1].Value];
    auto &rd = Regs[Ops[2].Value];
    rd = 0;
    auto DepCount {0};
    for (unsigned Bit = 0; Bit < 32; ++Bit) { // TODO little optimization is doable
      auto Masked = rs2 & (1 << Bit);
      if (Masked) {
        if (rs1 & (1 << DepCount)) {
          rd |= Masked;
        }
        ++DepCount;
      }
    }
    PC += 4;
  };
  TempTable[OpCodeST] = [](ToySim::Instruction &DecodedInstruction, std::vector<unsigned> &Regs, std::vector<unsigned> &Memory, std::vector<ToySim::Operand> &Ops, unsigned &PC) {
    assert(Ops.size() == 3);
    assert(Ops[0].OperandType == ImmEncoding);
    assert(Ops[1].OperandType == RegEncoding);
    assert(Ops[2].OperandType == RegEncoding);
    auto &imm = Ops[0].Value;                         // TODO memcpy
    auto &rt = Regs[Ops[1].Value];                    // TODO memory interfaces
    auto &base = Regs[Ops[2].Value];                  // TODO fix addressing
    Memory[base + signExtend(imm, Ops[0].Size)] = rt; // TODO bitwise memory
    PC += 4;                                                 // TODO aliasing check
  };
  return TempTable;
};
// TO BE GENERATED


class SPU {
private:

  // TODO Instructions in memory
  // TODO SPU State struct
  // Memory speces
  static const unsigned MemorySize = 1024;
  unsigned ProgramStartAddress;
  unsigned ArgumentsStartAddress;
  // TODO separate class
  static constexpr std::array<InstructionHandler, OpCodeMax> InstructionTable = initInstructionTable();

  
  struct SPUState {
    std::array<unsigned, kToyISARegNum> Regs;
    unsigned PC;
  };

  enum class SegmentPermission {
    Execute,
    Read,
    Write,
  };

  class SPUMemory {
  private:
    static const unsigned kOctWordSize = 32;
    static const unsigned kQuadWordSize = 16;
    static const unsigned kDoubleWordSize = 8;
    static const unsigned kWordSize = 4;
    static const unsigned kHalfWordSize = 2;
    static const unsigned kByteSize = 1;

    std::unordered_map<unsigned, char> Segments;
    public:
    std::array<char, MemorySize> Memory;
    static const unsigned kExecutableSegmentStartAddress = 400;
    void write(const void* Source, unsigned N, unsigned DestinationAddress) {
      assert(Source && "Invalid source pointer\n");
      assert((DestinationAddress < MemorySize && DestinationAddress + N < MemorySize) && "Trying to write to unreachable address\n");
      unsigned SourceBufferIndex {0};
      while (N > kOctWordSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kOctWordSize);
        N -= kOctWordSize;
        SourceBufferIndex += kOctWordSize;
      }

      while (N > kQuadWordSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kQuadWordSize);
        N -= kQuadWordSize;
        SourceBufferIndex += kQuadWordSize;
      }

      while (N > kDoubleWordSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kDoubleWordSize);
        N -= kDoubleWordSize;
        SourceBufferIndex += kDoubleWordSize;
      }

      while (N > kWordSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kWordSize);
        N -= kWordSize;
        SourceBufferIndex += kWordSize;
      }

      while (N > kHalfWordSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kHalfWordSize);
        N -= kHalfWordSize;
        SourceBufferIndex += kHalfWordSize;
      }
      
      while (N > kByteSize) {
        memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kByteSize);
        N -= kByteSize;
        SourceBufferIndex += kByteSize;
      }
    }

    void read(void* Destination, unsigned N, unsigned SourceAddress) const {
      assert(Destination && "Invalid destination pointer\n");
      assert((SourceAddress < MemorySize && SourceAddress + N < MemorySize) && "Trying to read from unreachable address\n");
      unsigned DestBufferIndex {0};
      while (N > kOctWordSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kOctWordSize);
        N -= kOctWordSize;
        DestBufferIndex += kOctWordSize;
      }

      while (N > kQuadWordSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kQuadWordSize);
        N -= kQuadWordSize;
        DestBufferIndex += kQuadWordSize;
      }

      while (N > kDoubleWordSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kDoubleWordSize);
        N -= kDoubleWordSize;
        DestBufferIndex += kDoubleWordSize;
      }

      while (N > kWordSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kWordSize);
        N -= kWordSize;
        DestBufferIndex += kWordSize;
      }

      while (N > kHalfWordSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kHalfWordSize);
        N -= kHalfWordSize;
        DestBufferIndex += kHalfWordSize;
      }
      
      while (N > kByteSize) {
        memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kByteSize);
        N -= kByteSize;
        DestBufferIndex += kByteSize;
      }
    }
  };

  SPUMemory RAM;
  SPUState State = {{}, 0};

public:
  SPU(const std::string& FileName, std::vector<unsigned> &Arguments) {
    std::ifstream File(FileName, std::ios::binary);
    if (!File.is_open()) {
        assert(!"Cannot open bin file\n");
    }
    
    File.seekg(0, std::ios::end);
    std::streamsize Size = File.tellg();
    File.seekg(0, std::ios::beg);

    // Not enough memory to load program
    if (MemorySize <= Size) {
      assert(0);
    }
    assert(Size % sizeof(int) == 0);
    File.read(reinterpret_cast<char*>(RAM.Memory.data() + RAM.kExecutableSegmentStartAddress), Size);
    File.close();

    State.PC = RAM.kExecutableSegmentStartAddress;

    ArgumentsStartAddress = 0;
    ProgramStartAddress = Arguments.size() * sizeof(unsigned);
    // TODO memory parameters that are defined by cli (again, CLI11 is needed)
    // FIXME for now address of arguments' list always starts with zero. 
  }

  Instruction decode(uint32_t BinInstruction) const;
  
  void compute();
  void regDump(unsigned N = 32) const;
  void memoryDump() const;
  void stateDump() const;
};

} // namespace ToySim

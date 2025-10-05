#include <cassert>
#include <vector>

#include "architecture.h"

// TODO: syscall support
// TODO: labels for cond. branches
// TODO: semantics layouts
// TODO: threaded code

namespace ToySim {
  void SPU::Compute() {
    assert(!BinInstructions.empty());
    auto BinInstruction = BinInstructions[0];
    while (true) {
      Instruction DecodedInstruction = Decode(BinInstruction);
      auto &Ops = DecodedInstruction.Operands; 
      switch (DecodedInstruction.OpCode) {
        case OpCodeJ: {
          assert(Ops.size() == 1);
          assert(Ops[0].OperandType == ImmEncoding);
          PC += Ops[0].Value;
          break;
        } case OpCodeMOVN: {
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
          break;
        } case OpCodeRBIT: {
          assert(Ops.size() == 2);
          assert(Ops[0].OperandType == RegEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          auto &rs = Regs[Ops[0].Value];
          auto &rd = Regs[Ops[1].Value];
          auto ReversedReg = Reverse(rs);
          rd = ReversedReg;
          PC += 4;
          break;
        } case OpCodeADD: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == RegEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &rd = Regs[Ops[0].Value];
          auto &rt = Regs[Ops[1].Value];
          auto &rs = Regs[Ops[2].Value];
          rd = rs + rt;
          PC += 4;
          break;
        } case OpCodeSLTI: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == ImmEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &imm = Ops[0].Value;
          auto &rt = Regs[Ops[1].Value];
          auto &rs = Regs[Ops[2].Value];
          rt = (rs < signExtend(imm, Ops[0].Size));
          PC += 4;
          break;
        } case OpCodeLD: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == ImmEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &imm = Ops[0].Value;
          auto &rt = Regs[Ops[1].Value];
          auto &base = Regs[Ops[2].Value];
          rt = Memory[base + signExtend(imm, Ops[0].Size)];
          PC += 4;
          break;
        } case OpCodeSYSCALL: {
          // TODO
          PC += 4;
          break;
        } case OpCodeCBIT: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == ImmEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &imm = Ops[0].Value;
          auto &rs = Regs[Ops[1].Value];
          auto &rd = Regs[Ops[2].Value];
          rd = rs & (~(1 << (imm - 1)));
          PC += 4;
          break;
        } case OpCodeSTP: {
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
          break;
        } case OpCodeBNE: {
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
          break;
        } case OpCodeUSAT: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == ImmEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &imm = Ops[0].Value;
          auto &rd = Regs[Ops[1].Value];
          auto &rs = Regs[Ops[2].Value];
          rd = saturateUnsigned(rs, imm);
          PC += 4;
          break;
        } case OpCodeBEQ: {
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
          break;
        } case OpCodeBDEP: {
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
          break;
        } case OpCodeST: {
          assert(Ops.size() == 3);
          assert(Ops[0].OperandType == ImmEncoding);
          assert(Ops[1].OperandType == RegEncoding);
          assert(Ops[2].OperandType == RegEncoding);
          auto &imm = Ops[0].Value;
          auto &rt = Regs[Ops[1].Value];
          auto &base = Regs[Ops[2].Value];
          Memory[base + signExtend(imm, Ops[0].Size)] = rt; 
          PC += 4;
          break;
        } default: {
          assert(!"Unsupported OpCode\n");
        }
      }
    }
  }

  Instruction SPU::Decode(int BinInstruction) const {
    OpCodes OpCode = (OpCodes)(BinInstruction & High6bitMask); // TODO comment types
    if (OpCode == 0) {
      OpCode = (OpCodes)(BinInstruction & Low6bitMask); // TODO comment types
    }
    auto Layout = Layouts.at(OpCode);

    auto BinLayout = Layout.second; // TODO struct maybe

    // TODO check ariphmetics if it bugs
    std::vector<Operand> Operands;
    auto CurrentBit{0};
    for (auto &Part: BinLayout) {
      if (Part.PartCode == OpCodeEncoding || Part.PartCode == ZeroEncoding) {
        CurrentBit += Part.PartSize;
        continue;
      }

      auto Value = (BinInstruction >> CurrentBit) & (1 << Part.PartSize);
      Operands.push_back({Value, Part.PartCode, Part.PartSize});
      CurrentBit += Part.PartSize;
    }

    Instruction DecodedInstruction = {OpCode, Operands};
    return DecodedInstruction;
  }

  int SPU::Reverse(int Val) { // TODO for N bits
    Val = (Val & 0xFFFF0000) >> 16 | (Val & 0x0000FFFF) << 16;
    Val = (Val & 0xFF00FF00) >> 8  | (Val & 0x00FF00FF) << 8;
    Val = (Val & 0xF0F0F0F0) >> 4  | (Val & 0x0F0F0F0F) << 4;
    Val = (Val & 0xCCCCCCCC) >> 2  | (Val & 0x33333333) << 2;
    Val = (Val & 0xAAAAAAAA) >> 1  | (Val & 0x55555555) << 1;
    return Val;
  }

  int SPU::signExtend(int Val, int N) {
    if (Val & (1 << (N - 1))) {
      int Mask = ~((1 << N) - 1);
      Val |= Mask;
    }
    return Val;
  }

  int SPU::saturateUnsigned(unsigned Val, unsigned N) {
    unsigned Limit = (1 << N) - 1;
    if (Val > Limit) {
      Val = Limit;
    }
    return Val;
  }
} // namespace ToySim

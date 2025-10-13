#include <cassert>
#include <iterator>
#include <vector>
#include <iostream>

#include "architecture.h"

// TODO: syscall support
// TODO: labels for cond. branches
// TODO: semantics layouts
// TODO: threaded code

namespace ToySim {
  void SPU::compute() {
    assert(!BinInstructions.empty());
    while (PC / 4 <= BinInstructions.size() - 1) { // TODO exit condition
      auto BinInstruction = BinInstructions[PC / 4];
      Instruction DecodedInstruction = decode(BinInstruction);
      auto &Ops = DecodedInstruction.Operands; 
      InstructionTable[DecodedInstruction.OpCode](DecodedInstruction, Regs, Memory, Ops, PC);
      //regDump(8);
    }
  }

  Instruction SPU::decode(int BinInstruction) const {
    //std::cout << "Decoding instruction: " << BinInstruction << '\n';
    OpCodes OpCode = (OpCodes)((BinInstruction & High6bitMask) >> 26); // TODO comment types
    if (OpCode == 0) {
      OpCode = (OpCodes)(BinInstruction & Low6bitMask); // TODO comment types
    }
    //std::cout << "Opcode = " << OpCode << '\n';
    auto Layout = Layouts.at(OpCode);

    auto BinLayout = Layout.second; // TODO struct maybe

    // TODO check ariphmetics if it bugs (it actually bugged)
    std::vector<Operand> Operands;
    auto CurrentBit{0};
    for (auto Part = BinLayout.rbegin(); Part < BinLayout.rend(); Part = std::next(Part)) {
      if (Part->PartCode == OpCodeEncoding || Part->PartCode == ZeroEncoding) {
        CurrentBit += Part->PartSize;
        continue;
      }
      auto Value = (BinInstruction >> CurrentBit) & ((1 << Part->PartSize) - 1);
      Operands.push_back({Value, Part->PartCode, Part->PartSize});
      CurrentBit += Part->PartSize;
    }

    Instruction DecodedInstruction = {OpCode, Operands};
    // std::cout << "Instruction decoded.\n";
    // std::cout << "Operands: ";
  //  for (auto &Op : Operands) {
      //std::cout << Op.Value << " ";
//    }
    //std::cout << '\n';
    return DecodedInstruction;
  }

  void SPU::memoryDump() const {
    std::cout << "### SPU memory dump: ###\n";
    for (unsigned I = 0; I < Memory.size(); ++I) {
      std::cout << "[" << I << "]: " << Memory[I] << '\n';
    }
    std::cout << "########################\n\n";
  }

  void SPU::regDump(unsigned N) const {
    std::cout << "[PC] = " << PC << '\n';
    if (N > RegNum) {
      N = RegNum;
    }
    for (unsigned RegCounter = 0; RegCounter < N; ++RegCounter) {
      std::cout << "[x" << RegCounter << "] = " << Regs[RegCounter] << '\n';
    }
  }

} // namespace ToySim

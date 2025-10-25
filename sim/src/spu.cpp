#include <cassert>
#include <cstdint>
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
    while (State.PC < RAM.Memory.size()) { // TODO exit condition
      uint32_t BinInstruction;
      RAM.read(&BinInstruction, sizeof(BinInstruction), State.PC);
      Instruction DecodedInstruction = decode(BinInstruction);
      InstructionTable[DecodedInstruction.OpCode](DecodedInstruction, State, RAM); // TODO SPU state
    }
  }

  Instruction SPU::decode(uint32_t BinInstruction) const {
    OpCodes OpCode = (OpCodes)((BinInstruction & High6bitMask) >> 26); // TODO comment types
    if (OpCode == 0) {
      OpCode = (OpCodes)(BinInstruction & Low6bitMask); // TODO comment types
    }
    auto Layout = Layouts.at(OpCode);;

    std::vector<Operand> Operands;
    auto CurrentBit{0};
    for (auto Part = Layout.rbegin(); Part != Layout.rend(); ++Part) { // != for generalization
      if (Part->PartCode == OpCodeEncoding || Part->PartCode == ZeroEncoding) {
        CurrentBit += Part->PartSize;
        continue;
      }
      auto Value = (BinInstruction >> CurrentBit) & ((1 << Part->PartSize) - 1);
      Operands.push_back({Value, Part->PartCode, Part->PartSize});
      CurrentBit += Part->PartSize;
    }

    Instruction DecodedInstruction = {OpCode, Operands};
    return DecodedInstruction;
  }

  void SPU::memoryDump() const {
    std::cout << "### SPU memory dump: ###\n";
    for (unsigned I = 0; I < RAM.kExecutableSegmentStartAddress; ++I) {
      uint32_t Value;
      RAM.read(&Value, sizeof(Value), I * sizeof(Value));
      std::cout << "[" << I << "]: " << Value << '\n';
    }
    std::cout << "########################\n\n";
  }

  void SPU::regDump(unsigned N) const {
    std::cout << "[PC] = " << State.PC << '\n';
    if (N > State.Regs.size()) {
      N = State.Regs.size();
    }
    for (unsigned RegCounter = 0; RegCounter < N; ++RegCounter) {
      std::cout << "[x" << RegCounter << "] = " << State.Regs[RegCounter] << '\n';
    }
  }

} // namespace ToySim

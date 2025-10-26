#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>

#include "architecture.h"

namespace ToySim {
  void SPU::compute() {
    while (State.PC < RAM.Memory.size()) {
      uint32_t BinInstruction;
      RAM.read(&BinInstruction, sizeof(BinInstruction), State.PC);
      Instruction DecodedInstruction = decode(BinInstruction);
      ToySim::SPU::SPUInstructionTable.at(DecodedInstruction.OpCode)(DecodedInstruction, RAM, State);
      if (State.Finished) {
        break;
      }
    }
  }

  Instruction SPU::decode(uint32_t BinInstruction) const {
    OpCodes OpCode = (OpCodes)((BinInstruction & High6bitMask) >> 26);
    if (OpCode == 0) {
      OpCode = (OpCodes)(BinInstruction & Low6bitMask);
    }
    auto Layout = Layouts.at(OpCode);;

    std::vector<Operand> Operands;
    auto CurrentBit{0};
    for (auto Part = Layout.rbegin(); Part != Layout.rend(); ++Part) {
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

  void SPU::SPUMemory::write(const void* Source, unsigned N, unsigned DestinationAddress) {
    assert(Source && "Invalid source pointer\n");
    assert((DestinationAddress < MemorySize && DestinationAddress + N < MemorySize) && "Trying to write to unreachable address\n");
    memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress, Source, N);
  }

  void SPU::SPUMemory::read(void* Destination, unsigned N, unsigned SourceAddress) const {
    assert(Destination && "Invalid destination pointer\n");
    assert((SourceAddress < MemorySize && SourceAddress + N < MemorySize) && "Trying to read from unreachable address\n");
    memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress, N);
  }


} // namespace ToySim

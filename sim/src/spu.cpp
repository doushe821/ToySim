#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>
#include <iostream>

#include "architecture.h"

namespace ToySim {
  void SPU::compute() {
    while (State.PC < RAM.Memory.size()) { // TODO exit condition
      uint32_t BinInstruction;
      RAM.read(&BinInstruction, sizeof(BinInstruction), State.PC);
      Instruction DecodedInstruction = decode(BinInstruction);
      if(DecodedInstruction.OpCode == OpCodeSYSCALL) {
        (SyscallTable[State.Regs[8]])(this, RAM, State);
      } else {
        InstructionTable[DecodedInstruction.OpCode](DecodedInstruction, RAM, State); // TODO SPU state
      }
        if (Finished) {
        break;
      }
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

  void SPU::SPUMemory::write(const void* Source, unsigned N, unsigned DestinationAddress) {
    assert(Source && "Invalid source pointer\n");
    assert((DestinationAddress < MemorySize && DestinationAddress + N < MemorySize) && "Trying to write to unreachable address\n");
    unsigned SourceBufferIndex {0};
    while (N >= kOctWordSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kOctWordSize);
      N -= kOctWordSize;
      SourceBufferIndex += kOctWordSize;
    }

    while (N >= kQuadWordSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kQuadWordSize);
      N -= kQuadWordSize;
      SourceBufferIndex += kQuadWordSize;
    }

    while (N >= kDoubleWordSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kDoubleWordSize);
      N -= kDoubleWordSize;
      SourceBufferIndex += kDoubleWordSize;
    }

    while (N >= kWordSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kWordSize);
      N -= kWordSize;
      SourceBufferIndex += kWordSize;
    }

    while (N >= kHalfWordSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kHalfWordSize);
      N -= kHalfWordSize;
      SourceBufferIndex += kHalfWordSize;
    }
    
    while (N >= kByteSize) {
      memcpy(reinterpret_cast<char*>(Memory.data()) + DestinationAddress + SourceBufferIndex, Source, kByteSize);
      N -= kByteSize;
      SourceBufferIndex += kByteSize;
    }
  }

  void SPU::SPUMemory::read(void* Destination, unsigned N, unsigned SourceAddress) const {
    assert(Destination && "Invalid destination pointer\n");
    assert((SourceAddress < MemorySize && SourceAddress + N < MemorySize) && "Trying to read from unreachable address\n");
    unsigned DestBufferIndex {0};
    while (N >= kOctWordSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kOctWordSize);
      N -= kOctWordSize;
      DestBufferIndex += kOctWordSize;
    }

    while (N >= kQuadWordSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kQuadWordSize);
      N -= kQuadWordSize;
      DestBufferIndex += kQuadWordSize;
    }

    while (N >= kDoubleWordSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kDoubleWordSize);
      N -= kDoubleWordSize;
      DestBufferIndex += kDoubleWordSize;
    }

    while (N >= kWordSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kWordSize);
      N -= kWordSize;
      DestBufferIndex += kWordSize;
    }

    while (N >= kHalfWordSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kHalfWordSize);
      N -= kHalfWordSize;
      DestBufferIndex += kHalfWordSize;
    }
    
    while (N >= kByteSize) {
      memcpy(Destination, reinterpret_cast<const char*>(Memory.data()) + SourceAddress + DestBufferIndex, kByteSize);
      N -= kByteSize;
      DestBufferIndex += kByteSize;
    }
  }


} // namespace ToySim

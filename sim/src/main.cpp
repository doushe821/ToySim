#include "architecture.h"
#include <cassert>
int main(int argc, char** argv) {
  std::vector<int> InitMemory = {1, -1, std::atoi(argv[1])};
  ToySim::SPU Sim {"output.bin", InitMemory, 32, 4};
  if (argc > 2) {
    if (std::atoi(argv[2]) == 1) { // TODO cli11
      Sim.memoryDump();
    }
  }
  Sim.compute();
  return 0;
}

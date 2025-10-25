#include "architecture.h"
#include <cassert>
int main(int argc, char** argv) {
  std::vector<unsigned> InitMemory = {1, (unsigned)-1, (unsigned)std::atoi(argv[2])};
  ToySim::SPU Sim {argv[1], InitMemory};
  if (argc > 3) {
    if (std::atoi(argv[3]) == 1) { // TODO cli11 for verbosity options
      Sim.memoryDump();
    }
  }
  Sim.compute();
  return 0;
}

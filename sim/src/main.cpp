#include "architecture.h"
#include <iostream>
int main(int argc, char** argv) {
  std::vector<int> InitMemory = {1, -1, std::atoi(argv[1])};
  ToySim::SPU Sim {"output.bin", InitMemory, 32, 4};
  //std::cout << "SPU initialized\n";
  Sim.memoryDump();
  Sim.compute();
  //std::cout << "Computed\n";
  //Sim.regDump(9);
  return 0;
}
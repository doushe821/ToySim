#include "architecture.h"
#include <iostream>
int main() {
  ToySim::SPU Sim {"output.bin"};
  std::cout << "SPU initialized\n";
  Sim.Compute();
  std::cout << "Computed\n";
  Sim.RegDump();
  return 0;
}
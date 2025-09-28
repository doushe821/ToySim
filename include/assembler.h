#pragma once
#include "architecture.h"
#include <string>
#include <fstream>
#include <iostream>
namespace ToySim {
namespace Asm {

class Assembler {
private:
  
public: 
  std::string assemble(std::fstream SourceCode);
};

} // namespace Asm
} // namespace ToySim

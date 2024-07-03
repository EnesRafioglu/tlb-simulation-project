#include "memory.hpp"

using namespace sc_core;

Memory::Memory(sc_module_name name) : sc_module(name) {
  // Register the Memory operation method and sensitivity list
  SC_THREAD(memoryOperation);

  sensitive << clk.pos() << writeEnabled << address << dataIn;
}

void Memory::memoryOperation() {
  while (true) {
    wait();

    // Write to memory on positive edge and write enabled
    if (clk.posedge() && writeEnabled) {
      memory[address] = dataIn;
    }

    // Read from memory and set dataOut to the value stored at address
    dataOut = memory[address];
  }
}

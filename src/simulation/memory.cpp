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

    const uint32_t addressValue = address.read().to_uint();

    // Write to memory on positive edge and write enabled
    if (clk.posedge() && writeEnabled) {
      memory[addressValue] = dataIn.read().to_uint();
    }

    // Read from memory and set dataOut to the value stored at address
    dataOut = memory[addressValue];
  }
}

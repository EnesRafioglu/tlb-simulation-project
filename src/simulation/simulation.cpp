#include <systemc>
#include <iostream>

#include "simulation.hpp"

#include "tlb.hpp"
#include "memory.hpp"
#include "mmu.hpp"
#include "cpu.hpp"

using namespace sc_core;


extern "C" Result run_simulation(
  int cycles,
  unsigned tlbSize,
  unsigned tlbsLatency,
  unsigned blocksize,
  unsigned v2bBlockOffset,
  unsigned memoryLatency,
  size_t numRequests,
  Request requests [],
  const char* tracefile
) {
  #ifdef DEBUG
  std::cout << "Running simulation with the following parameters:\n";
  std::cout << "\tCycles: " << cycles << "\n";
  std::cout << "\tTLB Size: " << tlbSize << "\n";
  std::cout << "\tTLB Latency: " << tlbsLatency << "\n";
  std::cout << "\tBlocksize: " << blocksize << "\n";
  std::cout << "\tV2B Block Offset: " << v2bBlockOffset << "\n";
  std::cout << "\tMemory Latency: " << memoryLatency << "\n";
  std::cout << "\tNumber of Requests: " << numRequests << "\n";
  std::cout << "\tTracefile: " << (tracefile != nullptr ? tracefile : "disabled") << "\n";
  std::cout << "\tRequests:\n";

  for (size_t i = 0; i < numRequests; i++) {
    std::cout << "\t\t" << i << ": addr=" << requests[i].addr << ", data=" << requests[i].data << ", write=" << requests[i].we << "\n";
  }

  std::cout << std::endl;
  #endif

  //* Create SystemC modules
  // Create the TLB
  TLB tlb("tlb", tlbSize, blocksize, v2bBlockOffset);

  // Create the Memory
  Memory memory("memory");

  // Create the MMU
  MMU mmu("mmu", &tlb, &memory, tlbsLatency, memoryLatency);

  // Create the CPU
  CPU cpu("cpu", &mmu, numRequests, requests);

  //* Wire up the SystemC modules
  sc_clock clk("clk", sc_time(1, SC_NS));
  memory.clk(clk);
  cpu.clk(clk);

  //* Run the simulation
  std::cout << "Starting simulation with " << cycles << " cycles" << std::endl;
  sc_start(cycles, SC_NS);

  unsigned int numCycles = sc_time_stamp().value() / 1000;

  // Return a dummy result
  return Result{numCycles, tlb.getNumMisses(), tlb.getNumHits(), 0};
}

// Leave this function as-is, it is required by the SystemC library but will not be used.
int sc_main(int argc, char* argv[]) {
  std::cerr << "Running the SystemC entrypoint function `sc_main` directly is not supported." << std::endl;
  
  return 1;
}

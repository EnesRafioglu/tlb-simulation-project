#include <systemc>
#include <iostream>

#include "simulation.hpp"

#include "tlb.hpp"
#include "memory.hpp"
#include "mmu.hpp"
#include "cpu.hpp"

using namespace sc_core;

size_t gateCount (unsigned tlbSize, unsigned blocksize) {
  unsigned int numIndexBits = std::log2(tlbSize);
  unsigned int numCachelines = tlbSize;
  unsigned int numTagBits = 32 - numIndexBits;

  unsigned int frameNumberBits = 32 - std::log2(blocksize);

  // the cache line might store the tag the frameNumber and a valid bit
  unsigned int cacheLineSize = numTagBits + frameNumberBits + 1;

  size_t cacheStoreGates = numCachelines * cacheLineSize * 4;	
  // the multiplexer is used to propagate the right cache entry.
  // From the ERA lecture it is known that a n-Bit Mux (with two inputs)
  // can be implemented with 3n + 1 gates. For more inputs k we can simply
  // take k-1 Muxes (in a tree like structure). Wo we need log2(k) select
  // bits and therefore log2(k) not gates.
  size_t muxGates = (numCachelines - 1) * (3 * cacheLineSize) + numIndexBits;

  // the decoder is used to select the right cache line with the index Bits as input
  // decoder with n inputs has 2^n outputs. Therefore we need 2^n and gates and n not gates.
  size_t decoderGates = (1 << numIndexBits) + numIndexBits;
  size_t overwriteGates = decoderGates + numCachelines;

  // compare the tag of the cache line with the tag of the virtual address
  // to check if virtual address is in the tlb. Compare is a SUB operation and 
  // a OR operation to check if the result is zero.
  // From ERA lecture it is known that a SUB operation can be implemented with 
  // a Full Adder (5n), where all bits of the second operand are inverted (n) 
  size_t comparatorGates = 6 * numTagBits + 1; 

  return cacheStoreGates + muxGates + decoderGates + overwriteGates + comparatorGates;
}

/**
 * @brief A flag indicating whether the simulation has run or not.
 *
 * This flag is used to keep track of whether the simulation has been executed or
 * not to prevent multiple executions of the simulation in the same SystemC
 * kernel instance.
 */
static bool didSimulationRun = false;

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
  // Ensure that the simulation is only run once with any given SystemC kernel instance
  if (didSimulationRun) {
    std::cerr << "Simulation has already been run. Please restart the simulation environment to run another simulation." << std::endl;
    exit(1);
  }
  didSimulationRun = true;

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

  // Calculate the bit parts of the virtual address
  // TODO: Check blocksize and tlbSize for power of 2
  // TODO: Blocksize must also be devisible by 4 and >= 4!
  const long numberOfPageTableEntries = 4294967296 / (long)blocksize;
  const unsigned int numPageBits = std::log2(numberOfPageTableEntries);
  const unsigned int numIndexBits = std::log2(tlbSize);
  const unsigned int numTagBits = numPageBits - numIndexBits;
  const unsigned int numOffsetBits = std::log2(blocksize);

  if(numTagBits + numIndexBits + numOffsetBits != 32) {
    std::cerr << "Address format does not add up to 32 bits" << std::endl;
    exit(1);
  };

  std::cout << "Address format: ";
  if (numTagBits > 0) {
    std::cout << "[" << std::string(numTagBits, 'T') << "]";
  }
  if (numIndexBits > 0) {
    std::cout << "[" << std::string(numIndexBits, 'I') << "]";
  }
  if (numOffsetBits > 0) {
    std::cout << "[" << std::string(numOffsetBits, 'O') << "]";
  }
  std::cout << "\n" << std::endl;
  #endif

  //* Create SystemC modules
  // Create the TLB
  CACHELINES cache("cache",tlbSize);

  //Create the TLB
  TLB tlb("tlb", &cache);

  // Create the Memory
  Memory memory("memory");

  // Create the MMU
  MMU mmu("mmu", &tlb, &memory, blocksize, v2bBlockOffset, tlbsLatency, memoryLatency);

  // Create the CPU
  CPU cpu("cpu", &mmu, numRequests, requests);

  //* Wire up the SystemC modules
  sc_clock clk("clk", sc_time(1, SC_NS));
  memory.clk(clk);
  cpu.clk(clk);

  //* Run the simulation
  std::cout << "Starting simulation with " << cycles << " cycles" << std::endl;
  sc_start(cycles, SC_NS);

  const size_t numElapsedCycles = sc_time_stamp().value() / 1000;

  size_t gc = gateCount(tlbSize, blocksize);

  // Return result
  //* NOTE: We record TLB hits and misses even if the request isn't fully
  //*       processed due to reaching the cycle limit. Our task description
  //*       doesn't specify that we should only count hits and misses for fully
  //*       processed requests. Counting them provides insight into TLB
  //*       efficiency, as they still occurred. Essentially, if the TLB request
  //*       is fully processed AND there is at least one cycle left for the result
  //*       to propagate to the MMU, we count the hit/miss. However, if the cycle
  //*       limit is reached during the TLB request, we don't count it.
  //*
  //*       The requirement for at least one cycle to still be left may seem a
  //*       bit arbitrary. After some deliberation, we based our decision on the
  //*       following reasoning:
  //*         - If the cycle limit is reached during the last cycle of `tlbsLatency`,
  //*           the response is not yet propagated and can be considered not fully
  //*           processed by the TLB.
  //*
  //*         - If the cycle limit is e.g., 10 and the MMU requests the TLB within
  //*           cycle 9 (say '9.2') with `tlbsLatency` of 1, the TLB should complete
  //*           at 10.2, which is outside the cycle limit, even though simply checking
  //*           10 - 9 >= 1 might suggest otherwise. This would require finer
  //*           granularity in cycle wait times, which is out of scope for this project.
  //*
  //*         - Counting such a hit/miss would require checking if the remaining
  //*           cycles are at least `tlbsLatency` long, and then incrementing the
  //*           hit/miss counter BEFORE waiting for the latency to finish. This seems
  //*           forced and goes against the spirit of SystemC, which ends the simulation
  //*           when the cycle limit is reached -> counters after the delay would not
  //*           be updated.
  return Result{
    cpu.isDone() ? numElapsedCycles : std::numeric_limits<size_t>::max(),
    mmu.getNumMisses(),
    mmu.getNumHits(),
    gc, 
  };
}

// Leave this function as-is, it is required by the SystemC library but will not be used.
int sc_main(int argc, char* argv[]) {
  // Suppress unused parameter warnings
  (void)argc;
  (void)argv;

  std::cerr << "Running the SystemC entrypoint function `sc_main` directly is not supported." << std::endl;
  
  return 1;
}

#include "tlb.hpp"

TLB::TLB(
  sc_module_name name,
  unsigned int tlbSize,
  unsigned int blocksize
) : sc_module(name), cacheLines("cache_lines", tlbSize) {
  // Calculate the number of bits used for the index and tag
  numIndexBits = std::log2(tlbSize);

  // Wire up signals
  cacheLines.tag(sigCacheLinesTag);
  cacheLines.index(sigCacheLinesIndex);
  cacheLines.frameNumberIn(frameNumberIn);
  cacheLines.writeEnabled(writeEnabled);
  cacheLines.frameNumberOut(frameNumberOut);
  cacheLines.found(found);

  // Register the tlb operation method and sensitivity list
  SC_THREAD(tlbOperation);

  sensitive << pageNumberIn << frameNumberIn << writeEnabled;
}

void TLB::tlbOperation() {
  while (true) {
    // Extract the tag and index from the page number
    sc_bv<32> tag = pageNumberIn.read().range(31, numIndexBits);
    sc_bv<32> index = numIndexBits > 0 ? pageNumberIn.read().range(numIndexBits - 1, 0) : sc_bv<32>(0);

    // Forward the tag and index to the cache lines
    sigCacheLinesTag.write(tag);
    sigCacheLinesIndex.write(index);

    wait();
  }
}

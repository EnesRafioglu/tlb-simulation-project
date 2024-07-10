#include "tlb.hpp"

#include <cmath>

#include "util.hpp"

TLB::TLB(
  sc_module_name name,
  unsigned int tlbSize,
  unsigned int blocksize
) : sc_module(name) {
  // Calculate the number of index bits
  numIndexBits = std::log2(tlbSize);

  // Register the TLB operation method and sensitivity list
  SC_THREAD(tlbOperation);

  sensitive << clk.pos() << virtualPageNumber;
}

void TLB::store(uint32_t virtualPage, uint32_t physicalFrame) {
  // Compute the cache location
  TLB::CacheLocation location = computeLocation(virtualPage);

  // Insert the cache entry into the cache
  tlbCache[location.index] = { location.tag, physicalFrame };

  #ifdef DEBUG
  std::cout << "TLB: Stored virtual page 0x" << std::hex << virtualPage << " with tag 0x" << location.tag << std::dec << " and index " << location.index << std::hex << " -> physical frame 0x" << physicalFrame << std::endl;
  #endif
}

TLB::CacheLocation TLB::computeLocation(uint32_t virtualPage) const {
  TLB::CacheLocation location;

  location.index = takeTrailingNBits(virtualPage, numIndexBits);
  virtualPage >>= numIndexBits;
  location.tag = virtualPage;

  return location;
}

void TLB::tlbOperation() {
  // TODO: Implement real TLB cache

  while (true) {
    // Compute the cache location
    TLB::CacheLocation location = computeLocation(virtualPageNumber.read().to_uint());

    auto it = tlbCache.find(location.index);

    #ifdef DEBUG
    std::cout << "TLB: Received virtual page 0x" << std::hex << virtualPageNumber.read() << " with tag 0x" << location.tag << std::dec << " and index " << location.index;

    if (it != tlbCache.end()) {
      std::cout << " -> Found cache entry with tag 0x" << it->second.tag;
    } else {
      std::cout << " -> No cache entry found";
    }
    #endif


    // Check if the cache entry exists and if the tag matches
    if (it != tlbCache.end() && it->second.tag == location.tag) {
      // Found a matching physical frame -> Hit
      physicalFrameNumber.write(it->second.frameNumber);
      found.write(true);

      #ifdef DEBUG
      std::cout << " -> Hit: 0x" << std::hex << it->second.frameNumber << std::endl;
      #endif
    } else {
      // No matching physical frame found -> Miss
      found.write(false);

      #ifdef DEBUG
      std::cout << " -> Miss" << std::endl;
      #endif
    }

    wait();
  }
}

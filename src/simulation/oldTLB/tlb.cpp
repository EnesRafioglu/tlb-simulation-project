#include "tlb.hpp"

#include <cmath>

#include "util.hpp"
#include "logging.hpp"

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

void TLB::store(const uint32_t virtualPage, const uint32_t physicalFrame) {
  // Compute the cache location
  TLB::CacheLocation location = computeLocation(virtualPage);

  // Insert the cache entry into the cache
  tlbCache[location.index] = { location.tag, physicalFrame };

  debug_log(
    "TLB",
    "Stored virtual page 0x" << std::hex << virtualPage << " with tag 0x" << location.tag << std::dec << " and index " << location.index << " -> physical frame 0x" << physicalFrame
  );
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
    const TLB::CacheLocation location = computeLocation(virtualPageNumber.read().to_uint());

    const auto it = tlbCache.find(location.index);

    debug_log(
      "TLB",
      "Received virtual page 0x" << std::hex << virtualPageNumber.read() << " with tag 0x" << location.tag << std::dec << " and index " << location.index
    );
    if (it != tlbCache.end()) {
      debug_log(
        "TLB",
        "Found cache entry with tag 0x" << it->second.tag
      );
    } else {
      debug_log(
        "TLB",
        "No cache entry found"
      );
    }

    // Check if the cache entry exists and if the tag matches
    if (it != tlbCache.end() && it->second.tag == location.tag) {
      // Found a matching physical frame -> Hit
      physicalFrameNumber.write(it->second.frameNumber);
      found.write(true);

      debug_log("TLB", "Hit: 0x" << std::hex << it->second.frameNumber);
    } else {
      // No matching physical frame found -> Miss
      found.write(false);

      debug_log("TLB", "Miss");
    }

    wait();
  }
}

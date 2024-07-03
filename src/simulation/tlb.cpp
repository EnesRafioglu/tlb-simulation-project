#include "tlb.hpp"

#include <cmath>

TLB::TLB(
  sc_module_name name,
  unsigned int tlbSize,
  unsigned int blocksize,
  unsigned int v2bBlockOffset
) : sc_module(name), blocksize(blocksize), v2bBlockOffset(v2bBlockOffset), numHits(0), numMisses(0) {
  // Calculate the number of offset and index bits

  // TODO: Check blocksize and tlbSize for power of 2
  numOffsetBits = std::log2(blocksize);
  numIndexBits = std::log2(tlbSize);

  // Register the TLB operation method and sensitivity list
  SC_THREAD(tlbOperation);

  sensitive << virtualAddress;
}

unsigned int TLB::getNumHits() const {
  return numHits;
}

unsigned int TLB::getNumMisses() const {
  return numMisses;
}

uint32_t TLB::store(uint32_t virtualAddress) {
  // Compute the cache location
  TLB::CacheLocation location = computeLocation(virtualAddress);
  
  // Normalize the virtual address to be the first address in the block
  uint32_t blockBaseAddress = virtualAddress & (~(blocksize - 1));

  // Compute the physical addresses
  auto physicalAddresses = std::make_unique<uint32_t[]>(blocksize);

  unsigned int baseOffset = blocksize * v2bBlockOffset;
  for (unsigned int i = 0; i < blocksize; i++) {
    physicalAddresses[i] = blockBaseAddress + baseOffset + i * 4;
  }

  // Save the matching physical address for later return
  uint32_t matchingPhysicalAddress = physicalAddresses[location.offset];

  // Create a new cache entry
  TLB::CacheEntry entry = { location.tag, std::move(physicalAddresses)};

  // Insert the cache entry into the cache
  tlbCache[location.index] = std::move(entry);

  #ifdef DEBUG
  std::cout << "TLB: Stored virtual address0x " << std::hex << virtualAddress << " with tag 0x" << location.tag << std::dec << ", index " << location.index << ", and offset " << location.offset << std::hex << " -> physical address 0x" << matchingPhysicalAddress << std::endl;
  #endif

  return matchingPhysicalAddress;
}

TLB::CacheLocation TLB::computeLocation(uint32_t virtualAddress) const {
  // Helper function to get the n least significant bits of a value
  auto takeNBits = [](uint32_t value, unsigned int n) {
    return value & ((1 << n) - 1);
  };

  TLB::CacheLocation location;

  location.offset = takeNBits(virtualAddress, numOffsetBits);
  virtualAddress >>= numOffsetBits;
  location.index = takeNBits(virtualAddress, numIndexBits);
  virtualAddress >>= numIndexBits;
  location.tag = virtualAddress;

  return location;
}

void TLB::tlbOperation() {
  // TODO: Implement real TLB cache

  while (true) {
    // Compute the cache location
    TLB::CacheLocation location = computeLocation(virtualAddress.read());

    auto it = tlbCache.find(location.index);

    #ifdef DEBUG
    std::cout << "TLB: Received virtual address 0x" << std::hex << virtualAddress.read() << " with tag 0x" << location.tag << std::dec << ", index " << location.index << ", and offset " << location.offset;

    if (it != tlbCache.end()) {
      std::cout << " -> Found cache entry with tag 0x" << it->second.tag;
    } else {
      std::cout << " -> No cache entry found";
    }
    #endif


    // Check if the cache entry exists and if the tag matches
    if (it != tlbCache.end() && it->second.tag == location.tag) {
      // Found a matching physical address -> Hit
      physicalAddress.write(it->second.physicalAddresses[location.offset]);
      found.write(true);

      #ifdef DEBUG
      std::cout << " -> Hit: 0x" << std::hex << it->second.physicalAddresses[location.offset] << std::endl;
      #endif

      numHits++;
    } else {
      // No matching physical address found -> Miss
      found.write(false);

      #ifdef DEBUG
      std::cout << " -> Miss" << std::endl;
      #endif

      numMisses++;
    }

    wait();
  }
}

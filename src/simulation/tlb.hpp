#pragma once

#include <map>
#include <memory>

#include <systemc>

using namespace sc_core;

/**
 * @brief SystemC module representing the Translation Lookaside Buffer (TLB).
 * 
 * The TLB is responsible for translating virtual addresses to physical addresses.
 * It contains a cache of previously translated addresses for faster lookup.
 */
SC_MODULE(TLB) {
public:
  SC_HAS_PROCESS(TLB);

  /** Virtual address that should be looked up */
  sc_in<uint32_t> virtualAddress;
  /** Possibly found matching physical address */
  sc_out<uint32_t> physicalAddress;
  /** Signal that indicates whether a matching physical address was found */
  sc_out<bool> found;

private:
  /**
   * @struct CacheLocation
   * @brief Represents a cache location with tag, index, and offset.
   */
  struct CacheLocation {
    /** The tag bits of the address */
    uint32_t tag;
    /** The index bits of the address */
    uint32_t index;
    /** The offset bits of the address */
    uint32_t offset;
  };

  /**
   * @struct CacheEntry
   * @brief Represents an entry in the cache.
   */
  struct CacheEntry {
    /** The tag bits of the address this entry belongs to */
    uint32_t tag;
    /** The physical addresses this entry points to */
    std::unique_ptr<uint32_t[]> physicalAddresses;
  };

  // TODO: Add real TLB cache implementation using SystemC
  /** Backing implementation of the TLB cache */
  std::map<uint32_t, CacheEntry> tlbCache;

  /** Size of a single cache set */
  unsigned int blocksize;
  /** Offset for virtual to block address translation */
  unsigned int v2bBlockOffset;

  /** Number of offset bits */
  unsigned int numOffsetBits;
  /** Number of index bits */
  unsigned int numIndexBits;

public:
  TLB(sc_module_name name, unsigned int tlbSize, unsigned int blocksize, unsigned int v2bBlockOffset);

  /**
   * @brief Stores a virtual address in the TLB.
   * 
   * This method will compute the corresponding physical address of the input
   * and its neighbots (according to the block size) and store them in the TLB.
   *
   * @param virtualAddress The virtual address to store in the TLB.
   * 
   * @return The physical address of the input virtual address.
   * 
   * @note The calculation of the physical address would probable be done by the
   * MMU in a real system. However, because of the dynamic block size, we simplify
   * the system and do the calculation in the TLB.
   */
  uint32_t store(uint32_t virtualAddress);

private:
  /**
   * @brief Computes the location of the virtual address in the TLB cache.
   *
   * @param virtualAddress The virtual address to compute the location for.
   * @return The location of the virtual address in the TLB cache.
   */
  CacheLocation computeLocation(uint32_t virtualAddress) const;

  /**
   * @brief Method that is called when a new virtual address is received.
   *
   * This method is responsible for looking up the virtual address in the TLB.
   * If a matching physical address is found, it is output on the `physicalAddress` port.
   * If no matching physical address is found, the `found` signal is set to false.
   */
  void tlbOperation();
};

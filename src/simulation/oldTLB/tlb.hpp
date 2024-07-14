#pragma once

#include <map>
#include <memory>

#include <systemc>

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief SystemC module representing the Translation Lookaside Buffer (TLB).
 * 
 * The TLB is responsible for translating virtual pages to physical frames.
 * It contains a cache of previously translated pages for faster lookup.
 * 
 * @note This implementation uses `uint32_t` for page and frame numbers as they
 * depend on the provided configuration and their actual width is determined at
 * runtime. A real implementation would use a fixed width for these numbers.
 */
SC_MODULE(TLB) {
public:
  SC_HAS_PROCESS(TLB);

  /** Clock used for processing requests */
  sc_in<bool> clk;

  /** Page number of the virtual address that should be looked up */
  sc_in<sc_bv<32>> virtualPageNumber;
  /** Possibly found matching physical frame number */
  sc_out<sc_bv<32>> physicalFrameNumber;
  /** Signal that indicates whether a matching physical frame was found */
  sc_out<bool> found;

private:
  /**
   * @struct CacheLocation
   * @brief Represents a cache location with tag and index.
   */
  struct CacheLocation {
    /** The tag bits of the address */
    uint32_t tag;
    /** The index bits of the address */
    uint32_t index;
  };

  /**
   * @struct CacheEntry
   * @brief Represents an entry in the cache.
   */
  struct CacheEntry {
    /** The tag bits of the page number this entry belongs to */
    uint32_t tag;
    /** The physical frame number this entry points to */
    uint32_t frameNumber;
  };

  // TODO: Add real TLB cache implementation using SystemC
  /** Backing implementation of the TLB cache */
  std::map<uint32_t, CacheEntry> tlbCache;

  /** Number of index bits */
  unsigned int numIndexBits;

public:
  TLB(sc_module_name name, unsigned int tlbSize, unsigned int blocksize);

  /**
   * @brief Stores a translation from virtual page to physical frame in the TLB.
   *
   * @param virtualPage The page number of the virtual address to store in the TLB.
   * @param physicalFrame The frame number of the physical address to store in the TLB.
   */
  void store(const uint32_t virtualPage, const uint32_t physicalFrame);

private:
  /**
   * @brief Computes the location of the virtual page in the TLB cache.
   *
   * @param virtualPage The page number of the virtual address to compute the location for.
   * @return The location of the virtual page in the TLB cache.
   */
  CacheLocation computeLocation(uint32_t virtualPage) const;

  /**
   * @brief Method that is called when a new virtual page is received.
   *
   * This method is responsible for looking up the virtual page in the TLB.
   * If a matching physical frame is found, it is output on the `physicalFrameNumber` port.
   * If no matching physical frame is found, the `found` signal is set to false.
   */
  void tlbOperation();
};

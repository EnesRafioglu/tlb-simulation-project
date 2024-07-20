#pragma once

#include <systemc>

#include "cache_lines.hpp"

using namespace sc_core;

/**
 * @brief SystemC module representing a Translation Lookaside Buffer (TLB).
 *
 * The TLB is a cache that stores mappings of virtual memory pages to physical
 * memory frames. This aids in the quick translation of virtual addresses to
 * physical addresses.
 */
SC_MODULE(TLB) {
public:
  SC_HAS_PROCESS(TLB);

  /** Page number of the virtual address to be looked up in the TLB */
  sc_in<sc_bv<32>> pageNumberIn;

  /** Frame number of the physical address to be stored in the TLB */
  sc_in<sc_bv<32>> frameNumberIn;

  /** Indicates whether `frameNumberIn` should be persisted in the TLB */
  sc_in<bool> writeEnabled;

  /** @brief Frame number currenlty stored in the TLB for `pageNumberIn`
   * 
   * This is only the frame number stored in the cache line that `pageNumberIn`
   * maps to. Before using this value, you should check if the frame was found
   * using the `found` signal.
   */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Indicates whether `pageNumberIn` was present in the TLB and
   * `frameNumberOut` is valid */
  sc_out<bool> found;

private:

  /** Underlying cache lines that hold the actual values in the TLB */
  CACHELINES cacheLines;

  // Signals for communication with the cache lines
  sc_buffer<sc_bv<32>> sigCacheLinesTag;
  sc_buffer<sc_bv<32>> sigCacheLinesIndex;

  /** Number of bits used for the index from `pageNumberIn` */
  int numIndexBits;

public:
  TLB(sc_module_name name, unsigned int tlbSize, unsigned int blocksize);

private:
  /** @brief Method that is called when a signal on the sensitivity list changes.
   * 
   * This method is responsible for extracting the tag and index from
   * `pageNumberIn` and forwarding the data to the cache lines.
   */
  void tlbOperation();
};

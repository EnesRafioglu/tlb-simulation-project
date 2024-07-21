#pragma once

#include <vector>
#include <memory>

#include <systemc>

#include "cache_line.hpp"

using namespace sc_core;

/**
 * @brief SystemC module representing a set of cache lines.
 *
 * This module contains multiple cache lines and orchestrates the read and write
 * operations on them.
 */
SC_MODULE(CACHELINES) {
public:
  SC_HAS_PROCESS(CACHELINES);

  /** Input port for the frame to lookup */
  sc_in<sc_bv<32>> tag;

  /** Input port for the index to select the specific cache line */
  sc_in<sc_bv<32>> index;

  /** Input port for the frame number data to be stored in the TLB */
  sc_in<sc_bv<32>> frameNumberIn;

  /** Input port for the write enable signal */
  sc_in<bool> writeEnabled;

  /** Output port for the frame number stored in the cache line */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Output port that indicates if the frame was found */
  sc_out<bool> found;

private:

  /**
   * @brief Represents a single entry in a list of cache lines.
   * 
   * This struct contains signals and data related to a cache line entry.
   * It includes the cache line itself, as well as signals unique to it.
   */
  struct CacheLineEntry {
  public:
    CACHELINE cacheline;

    sc_signal<bool> writeEnabled;

    sc_signal<sc_bv<32>> tagOut;
    sc_signal<sc_bv<32>> frameNumberOut;
    sc_signal<bool> isInitializedOut;

  public:
    CacheLineEntry();
  };

  //? Note on the usage of std::unique_ptr
  //?
  //? SystemC objects have their copy and move constructors either deleted or
  //? inaccessible for our means. This means that we can't store them in a
  //? std::vector directly. We can, however, store them in a std::unique_ptr
  //? and then store the std::unique_ptr in the vector. This way, we can
  //? dynamically allocate the objects and store them in the vector while still
  //? being memory safe.

  /** Cache lines orchestrated by the TLB */
  std::vector<std::unique_ptr<CacheLineEntry>> cacheLines;

  /** 
   * @brief Cache line that was last selected
   * 
   * A real implementation wouldn't need this, but as we simulate the decoder
   * that selects the cache line by indexing into the cacheLines vector, we need
   * to store the last selected index to be able to reset it's write enable
   * signal. We think that a decoder would be an unuseful abstraction in this
   * case.
   * */
  size_t lastSelectedIndex = 0;

public:
  CACHELINES(sc_module_name name, size_t numLines, sc_trace_file* vcdTraceFile);

private:
  /** @brief Method that is called when a signal on the sensitivity list changes.
   * 
   * This method is responsible for sending requests to the corresponding cache
   * line and building the response.
   */
  void cacheLinesOperation();
};

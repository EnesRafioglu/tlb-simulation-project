#pragma once

#include <systemc>

#include "cacheline.hpp"

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

  /** 
   * Struct to hold the signals for tagOut, frameNumberOut, and initializedOut
   * of a cache line.
   */
  struct LineSignals {
    /** Signal for write enable */
    sc_signal<bool> weSignal;

    /** @brief Signal indicating if cache line is initialized
     * 
     * (will be set to true in the constructor)
     */
    sc_signal<bool> initializedOutSignal;

    /** Signal for the tag data */
    sc_signal<sc_bv<32>> tagSignal;

    /** Signal for the frame number data */
    sc_signal<sc_bv<32>> frameNumberSignal;
  };

  /** Array of pointers to CacheLine modules. */
  CACHELINE** cache;

  /** Array of LineSignals for each CacheLine. */
  LineSignals* lineSignals;

  /** Input port for the tag data to be checked in the TLB. */
  sc_in<sc_bv<32>> tag;

  /** Input port for the index to select the specific cache line. */
  sc_in<sc_bv<32>> index;

  /** Input port for the frame number data to be stored in the TLB. */
  sc_in<sc_bv<32>> frameNumber;

  /** Input port for the write enable signal. */
  sc_in<bool> we;

  /** Output port for the frame number stored in the cache line */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Output port indicating whether the tag was found in the cache line */
  sc_out<bool> found;

  /** Size of the TLB */
  unsigned tlbsize;

public:
  CACHELINES(sc_module_name name, unsigned tlbsize);

  ~CACHELINES();

private:
  void update();
};

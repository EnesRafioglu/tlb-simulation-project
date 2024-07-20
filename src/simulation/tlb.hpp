#pragma once

#include <systemc>
#include <cmath>

#include "cachelines.hpp"
#include "util.hpp"

/**
 * @brief Wrapper of the CACHELINES Module.
 * 
 * This module splits the page number into tag and index and
 * passes it to the cache-field along with other inputs.
 */
SC_MODULE(TLB) {
public:
  SC_HAS_PROCESS(TLB);

  /** Input port for the virtual page number. */
  sc_in<sc_bv<32>> pageNumberIn;

  /** Input port for the frame number to be stored. */
  sc_in<sc_bv<32>> frameNumberIn;

  /** Input port for the write enable signal. */
  sc_in<bool> we;

  /** Pointer to the inner cache. */
  CACHELINES* cache;

  /** Output port for the frame number stored in the cache line. */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Output port indicating whether the tag was found in the cache line. */
  sc_out<bool> found;

  /** Signal for the tag. */
  sc_signal<sc_bv<32>> tagSignal;

  /** Signal for the index. */
  sc_signal<sc_bv<32>> indexSignal;

  /** Size of the TLB. */
  unsigned tlbsize;

public:
  TLB(sc_module_name name, CACHELINES* cache);

private:
  /** Thread process that updates the TLB. */
  void update();
};

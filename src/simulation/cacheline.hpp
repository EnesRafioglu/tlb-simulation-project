#pragma once

#include <systemc>

#include "register.hpp"

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief SystemC module representing a cache line.
 *
 * A cache line is a unit of data storage in a cache memory. It consists of a tag and a frame number.
 * This module contains input and output ports for the tag and frame number data, as well as a write enable signal.
 */
SC_MODULE(CACHELINE) {
public:
  SC_HAS_PROCESS(CACHELINE);

  /** Input port for the tag data to be stored in the cache line. */
  sc_in<sc_bv<32>> tagIn;

  /** Input port for the frame number data to be stored in the cache line. */
  sc_in<sc_bv<32>> dataIn;

  /** Input port for the write enable signal. */
  sc_in<bool> weIn;

  /** Output port for the tag data stored in the cache line. */
  sc_out<sc_bv<32>> tagOut;

  /** Output port for the frame number stored in the cache line. */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Register to store the tag data. */
  REGISTER tagStore;

  /** Register to store the frame number data. */
  REGISTER frameNumberStore;

  /** Signal to hold the intermediate tag data. */
  sc_signal<sc_bv<32>> tagSignal;

  /** Signal to hold the intermediate frame number data. */
  sc_signal<sc_bv<32>> frameNumberSignal;

public:
  CACHELINE(sc_module_name name);
};

#pragma once

#include <systemc>

#include "register.hpp"

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief SystemC module representing a cache line.
 *
 * A cache line is a unit of data storage in a cache memory.
 * It consists of a tag and a frame number as well as a flag to indicate if the
 * line was initialized.
 * 
 * This module does not contain any logic, it only encapsulates the data
 * storage.
 */
SC_MODULE(CACHELINE) {
public:
  SC_HAS_PROCESS(CACHELINE);

  /** Input port for the tag data to be stored in the cache line. */
  sc_in<sc_bv<32>> tagIn;

  /** Input port for the frame number data to be stored in the cache line. */
  sc_in<sc_bv<32>> dataIn;

  /** Input port for the write enable signal. */
  sc_in<bool> writeEnabled;

  /** Output port for the tag data stored in the cache line. */
  sc_out<sc_bv<32>> tagOut;

  /** Output port for the frame number stored in the cache line. */
  sc_out<sc_bv<32>> frameNumberOut;

  /** Output port for the initialized flag. */
  sc_out<bool> isInitializedOut;

private:
  /** Register to store the tag data. */
  REGISTER<sc_bv<32>> tagStore;

  /** Register to store the frame number data. */
  REGISTER<sc_bv<32>> frameNumberStore;

  /** Register to store if the line was initialized
   * 
   * In a real system, this could be implemented with a single D-flip-flop.
   * We still count this as a single flip-flop in our gate count, but opted to
   * use a register here for simplicity.
  */
  REGISTER<bool> initializedStore;

  /** @brief Signal that is always true
   * 
   * This signal is used to set the initializedStore register on the first write.
  */
  sc_signal<bool> constantTrueSignal;

public:
  CACHELINE(sc_module_name name);
};

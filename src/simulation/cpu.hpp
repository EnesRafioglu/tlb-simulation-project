#pragma once

#include <systemc>

#include "mmu.hpp"
#include "../shared_types.h"

using namespace sc_core;
using namespace sc_dt;

/**
 * @brief CPU module for simulating a CPU.
 *
 * The CPU module is responsible for sending requests to the MMU and receiving the results.
 */
SC_MODULE(CPU) {
public:
  SC_HAS_PROCESS(CPU);

  /** Clock used for sending requests */
  sc_in<bool> clk;

  /** Signal that indicates whether the current value of `dateIn` should be persisted */
  sc_out<bool> writeEnabled;
  /** Current virtual address all operations relate to */
  sc_out<sc_bv<32>> address;
  /** Data that should potentially be written to memory */
  sc_out<sc_bv<32>> dataIn;
  /** Data currently in the memory cell at `address` */
  sc_in<sc_bv<32>> dataOut;

private:
  /** Memory-Management-Unit used by the CPU to handle it's requests */
  MMU* mmu;

  /** Number of requests that the CPU should process */
  unsigned int numRequests;
  /** Array of requests that the CPU should process */
  Request* requests;
  /** Index of the current request */
  unsigned int requestIndex;

  //* Signals used for communication between the CPU and the MMU module
  // For documentation on the signals, see the corresponding port declarations above

  // We need to use `sc_buffer` for at leas one of the channels to the MMU to
  // trigger it's operation (sensitivity list).
  //
  // If we actually knew the latencies ahead of time, we wouldn't be forced to
  // use the `mmuDone` event to signal the CPU that the operation is done.
  // This signal is only notified after the MMU has finished it's operation, thus
  // we need to trigger the MMU in the first place.
  sc_signal<bool> sigWriteEnabled;
  sc_buffer<sc_bv<32>> sigAddress;
  sc_signal<sc_bv<32>> sigDataIn;
  sc_signal<sc_bv<32>> sigDataOut;

public:
  CPU(sc_module_name name, MMU* mmu, unsigned int numRequests, Request requests[]);

  /**
   * @brief Checks if the CPU is done with all it's requests.
   *
   * This function returns a boolean value indicating whether the CPU has
   * finished processing all of its requests.
   *
   * @return True if the CPU is done with its requests, false otherwise.
   */
  bool isDone() const;

private:
  /** @brief Method that is called on the positive edge of the clock signal.
   * 
   * This method is responsible for sending requests to the MMU and
   * retrieving the responses.
   */
  void cpuOperation();
};

#pragma once

#include <map>

#include <systemc>

using namespace sc_core;

/**
 * @brief Memory module for storing data.
 *
 * The Memory module is responsible for storing data in a map structure.
 * It has input ports for clock signal, write enable signal, address, and data input.
 * It also has an output port for data output.
 */
SC_MODULE(Memory) {
public:
  SC_HAS_PROCESS(Memory);

  /** Clock used for writing values into memory on positive edge */
  sc_in<bool> clk;

  /** Signal that indicates whether the current value of `dateIn` should be persisted */
  sc_in<bool> writeEnabled;
  /** Current address all operations relate to */
  sc_in<uint32_t> address;
  /** Data that should potentially be written to memory */
  sc_in<uint32_t> dataIn;
  /** Data currently in the memory cell at `address` */
  sc_out<uint32_t> dataOut;

private:
  /** @brief Used internally as the memory backend.
   * 
   * The key is the address and the value is the data stored at that address.
   * We don't implement the memory ourselves, as it is outside the scope of this project.
  */
  std::map<uint32_t, uint32_t> memory;

public:
  Memory(sc_module_name name);

private:
  /** @brief Method that is called on the positive edge of the clock signal.
   * 
   * This method is responsible for writing values into memory when `writeEnabled` is true.
   * It also reads values from memory and sets `dataOut` to the value stored at `address`.
  */
  void memoryOperation();
};

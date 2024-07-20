#pragma once

#include <systemc>

#include "logging.hpp"

using namespace sc_core;

//? Note on source code in a header file
//?
//? The REGISTER module is generic in the data type it stores. This means that
//? we either have to define explicit instantiations of the module in a source
//? file or write the implementation in the header file and let the compiler
//? generate the code for the specific data types we use. We opted for the
//? latter approach here to be more flexible.

/**
 * @brief SystemC module representing a register.
 *
 * A register is a unit of data storage in a computer. It consists of a data input and a data output.
 * This module contains input and output ports for the data, as well as a write enable signal.
 */
template <class DataType>
SC_MODULE(REGISTER) {
public:
  SC_HAS_PROCESS(REGISTER);

  /** Input port for the data to be written to the register. */
  sc_in<DataType> d;

  /** Input port for the write enable signal. */
  sc_in<bool> we;

  /** Output port for the data stored in the register. */
  sc_out<DataType> q;

private:

  /** Initial value stored in this register */
  DataType initialValue;

public:
  REGISTER(sc_module_name name, DataType initialValue) : sc_module(name) {
    // Set the initial value of the register
    this->initialValue = initialValue;

    // Register the register operation method and sensitivity list
    SC_THREAD(registerOperation);

    sensitive << we.pos();
  }

private:
  /** Thread process that updates the register value. */
  void registerOperation() {
    // Initialize the register with the initial value
    q.write(initialValue);

    while (true) {
      // Wait for the write enable signal before updating the register with any
      // values from the data input port
      wait();

      q.write(d.read());

      debug_log(
        "Register",
        sc_get_hierarchical_name(name()) << " updated to " << d.read()
      )
    }
  }
};

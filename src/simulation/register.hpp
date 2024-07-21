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

    //? We are using SC_METHOD instead of SC_THREAD here to avoid spwaning
    //? a huge number of threads, which would leed to:
    //?
    //? Warning: (W518) stack setup failed: unsuccessful stack protection
    //? ignored: Cannot allocate memory, address=0x, enable=true`
    //?
    //? from SystemC, as the default maximum number of memory mappings per
    //? process is 65536, which is not enough for the number of threads we
    //? would spawn. (Yes, this took an hour to debug...)
    SC_METHOD(registerOperation);

    dont_initialize();

    sensitive << we.pos();
  }

private:
  /** Initialize the register with the initial value.
   * 
   * Combined with `dont_initialize()` in the constructor, this method ensures
   * that the register is initialized with the initial value and not reading
   * garbage from the input port.
  */
  void end_of_elaboration() {
    q.write(initialValue);
  }

  /** Thread process that updates the register value. */
  void registerOperation() {
    q.write(d.read());

    debug_log(
      "Register",
      sc_get_hierarchical_name(name()) << " updated to " << d.read()
    )
  }
};

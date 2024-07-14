#pragma once

#include <systemc>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

/**
 * @brief Module that represents a register.
 *
 */

SC_MODULE(REGISTER){
 public:

 SC_HAS_PROCESS(REGISTER);
 /**
  * @brief Input port for the data to be written to the register.
  *
  * This port accepts a vector of bits of the specified size.
  */

 sc_in<sc_bv<32>>  d;

 /**
  * @brief Input port for the write enable signal.
  *
  * When this signal is on positive edge, the data on the `d` port is written to the register.
  */
 sc_in<bool> we;

 /**
  * @brief Output port for the data stored in the register.
  *
  * This port outputs the current value stored in the register.
  */
 sc_out<sc_bv<32>> q;

 /**
  * @brief Constructor.
  *
  * Sets the sensitivity list for the update() process
  *
  * @param name Name of the SystemC module.
  */
 REGISTER(sc_module_name name);

 private:
 /**
  * @brief Thread process that updates the register value.
  *
  * This process waits for the positive edge of the write enable signal and then updates the
  * register value with the data from the input port `d`.
  */
 void update();
};




SC_MODULE(FLIPFLOP) {

 sc_in<bool> d;
 sc_in<bool> we;
 sc_out<bool> q;

 SC_CTOR(FLIPFLOP) {
  SC_THREAD(run)
  sensitive << we.pos();
}
 void run() {
  while(true) {
   wait();
   q->write(d->read());
  }
 }


};




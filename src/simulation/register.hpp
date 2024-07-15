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
  */
 sc_in<sc_bv<32>>  d;

 /**
  * @brief Input port for the write enable signal.
  */
 sc_in<bool> we;

 /**
  * @brief Output port for the data stored in the register.
  */
 sc_out<sc_bv<32>> q;


 REGISTER(sc_module_name name);

 private:
 /**
  * @brief Thread process that updates the register value.
  *
  */
 void update();
};





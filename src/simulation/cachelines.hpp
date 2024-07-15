#pragma once

#include <systemc>
#include "cacheline.hpp"
using namespace sc_core;

SC_MODULE(CACHELINES) {

 SC_HAS_PROCESS(CACHELINES);

 /**
  * @brief Struct to hold the signals for tagOut, frameNumberOut, and initializedOut
  * of a cache line.
  */
 struct LineSignals {
  /** @brief Signal for write enable */
  sc_signal<bool> weSignal;

  /** @brief Signal indicating if cache line is initialized
   * (will be set to true in the constructor)
   */
  sc_signal<bool> initializedOutSignal;

  /** @brief Signal for the tag data */
  sc_signal<sc_bv<32>> tagSignal;

  /** @brief Signal for the frame number data */
  sc_signal<sc_bv<32>> frameNumberSignal;
 };

 /**
  * @brief Array of pointers to CacheLine modules.
  */
 CACHELINE** cache;

 /**
  * @brief Array of LineSignals for each CacheLine.
  */
 LineSignals* lineSignals;

 /**
  * @brief Input port for the tag data to be checked in the TLB.
  */
 sc_in<sc_bv<32>> tag;

 /**
  * @brief Input port for the index to select the specific cache line.
  */
 sc_in<sc_bv<32>> index;

 /**
  * @brief Input port for the frame number data to be stored in the TLB.
  */
 sc_in<sc_bv<32>> frameNumber;

 /**
  * @brief Input port for the write enable signal.
  */
 sc_in<bool> we;

 /**
  * @brief Output port for the frame number stored in the cache line
  */
 sc_out<sc_bv<32>> frameNumberOut;

 /**
  * @brief Output port indicating whether the tag was found in the cache line
  */
 sc_out<bool> found;

 /**
  * @brief Size of the TLB
  */
 unsigned tlbsize;

 CACHELINES(sc_module_name name, unsigned tlbsize);

 /**
  * @brief Destructor.
  */
 ~CACHELINES();

 void update();
};

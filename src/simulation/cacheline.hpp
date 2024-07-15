#pragma once

#include <systemc>
#include "register.hpp"

using namespace sc_core;
using namespace sc_dt;

/**
 * @file CACHELINE.hpp
 * @brief Header file for the CACHELINE module.
 */

SC_MODULE(CACHELINE) {

    SC_HAS_PROCESS(CACHELINE);
    public:

    /**
     * @brief Input port for the tag data to be stored in the cache line.
     */
    sc_in<sc_bv<32>> tagIn;

    /**
     * @brief Input port for the frame number data to be stored in the cache line.
     */
    sc_in<sc_bv<32>> dataIn;

    /**
     * @brief Input port for the write enable signal.
     */
    sc_in<bool> weIn;

    /**
     * @brief Output port for the tag data stored in the cache line.
     */
    sc_out<sc_bv<32>> tagOut;

    /**
     * @brief Output port for the frame number stored in the cache line.
     */
    sc_out<sc_bv<32>> frameNumberOut;


 CACHELINE(sc_module_name name);


    /**
     * @brief Register to store the tag data.
     */
    REGISTER tagStore;

    /**
     * @brief Register to store the frame number data.
     */
    REGISTER frameNumberStore;

    /**
     * @brief Signal to hold the intermediate tag data.
     */
    sc_signal<sc_bv<32>> tagSignal;

    /**
     * @brief Signal to hold the intermediate frame number data.
     */
    sc_signal<sc_bv<32>> frameNumberSignal;




};


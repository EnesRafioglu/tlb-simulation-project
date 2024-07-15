#pragma once

#include <systemc>
#include <cmath>
#include "cachelines.hpp"
#include "util.hpp"

using namespace sc_core;
using namespace sc_dt;

/**
 * @file newtlb.hpp
 * @brief Header file for the NEWTLB module.
 *
 * This file contains the declaration of the NEWTLB module.
 */

SC_MODULE(NEWTLB) {

    SC_HAS_PROCESS(NEWTLB);

    sc_in<sc_bv<32>> pageNumberIn;
    sc_in<sc_bv<32>> frameNumberIn;
    sc_in<bool> we;

    CACHELINES* cache;

    sc_out<sc_bv<32>> frameNumberOut;
    sc_out<bool> found;

    sc_signal<sc_bv<32>> tagSignal;
    sc_signal<sc_bv<32>> indexSignal;

    unsigned tlbsize;

    NEWTLB(sc_module_name name, CACHELINES* cache);

    void update();
};

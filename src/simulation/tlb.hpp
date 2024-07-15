#pragma once

#include <systemc>
#include <cmath>
#include "cachelines.hpp"
#include "util.hpp"



/**
 * @brief Wrapper of the CACHELINES Module.
 * This module splits the page number into tag and index and
 * passes it to the cache-field along with other inputs.
 */
SC_MODULE(TLB) {

    SC_HAS_PROCESS(TLB);

    /**
     * @brief Input port for the virtual page number.
     */
    sc_in<sc_bv<32>> pageNumberIn;

    /**
     * @brief Input port for the frame number to be stored.
     */
    sc_in<sc_bv<32>> frameNumberIn;

    /**
     * @brief Input port for the write enable signal.
     */
    sc_in<bool> we;

    /**
     * @brief Pointer to the inner cache.
     */
    CACHELINES* cache;

    /**
     * @brief Output port for the frame number stored in the cache line.
     */
    sc_out<sc_bv<32>> frameNumberOut;

    /**
     * @brief Output port indicating whether the tag was found in the cache line.
     */
    sc_out<bool> found;

    /**
     * @brief Signal for the tag.
     */
    sc_signal<sc_bv<32>> tagSignal;

    /**
     * @brief Signal for the index.
     */
    sc_signal<sc_bv<32>> indexSignal;

    /**
     * @brief Size of the TLB.
     */
    unsigned tlbsize;

    TLB(sc_module_name name, CACHELINES* cache);

    void update();
};


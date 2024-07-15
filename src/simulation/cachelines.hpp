#pragma once

#include <systemc>
#include "cacheline.hpp"
using namespace sc_core;



SC_MODULE(CACHELINES) {

    SC_HAS_PROCESS(CACHELINES);

    /**
     * @brief Struct to hold the signals for tagOut, frameNumberOut, and initializedOut.
     */
    struct LineSignals {
        sc_signal<bool> weSignal;
        sc_signal<sc_bv<32>> tagOutSignal;
        sc_signal<sc_bv<32>> frameNumberOutSignal;
        sc_signal<bool> initializedOutSignal;
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

    sc_out<sc_bv<32>> frameNumberOut;

    sc_out<bool> found;

    unsigned tlbsize;

    /**
     * @brief Constructor for the TLB module.
     * @param name Name of the SystemC module.
     */
    CACHELINES(sc_module_name name, unsigned tlbsize);

    /**
     * @brief Destructor for the TLB module.
     */
    ~CACHELINES();

    void update();
};


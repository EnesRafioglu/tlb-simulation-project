#pragma once

#include <systemc>
#include "cacheline.hpp"
using namespace sc_core;
using namespace std;

/**
 * @file TLB.hpp
 * @brief Header file for the TLB module.
 *
 * This file contains the declaration and definition of the TLB module.
 */

template<int tlbSize>
SC_MODULE(TLB) {

    SC_HAS_PROCESS(TLB);

    /**
     * @brief Struct to hold the signals for tagOut, frameNumberOut, and initializedOut.
     */
    struct LineSignals {
        sc_signal<sc_bv<32>> tagOutSignal;
        sc_signal<sc_bv<32>> frameNumberOutSignal;
        sc_signal<bool> initializedOutSignal;
    };

    /**
     * @brief Array of pointers to CacheLine modules.
     */
    CACHELINE** cacheLines;

    /**
     * @brief Array of signals for the write enable inputs of each CacheLine.
     */
    sc_signal<bool> weSignals[tlbSize];

    /**
     * @brief Array of LineSignals for each CacheLine.
     */
    LineSignals lineSignals[tlbSize];

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

    /**
     * @brief Constructor for the TLB module.
     * @param name Name of the SystemC module.
     */
    TLB(sc_module_name name) : sc_module(name) {

        cacheLines = new CACHELINE*[tlbSize];
        lineSignals = new LineSignals[tlbSize];

        for (int i = 0; i < tlbSize; ++i) {

            cacheLines[i] = new CACHELINE(("CacheLine_" + std::to_string(i)).c_str());
            cacheLines[i]->we(weSignals[i]);  // Bind the write enable signal
            cacheLines[i]->tagIn(tag);
            cacheLines[i]->dataIn(frameNumber);

            cacheLines[i]->tagOut(lineSignals[i].tagOutSignal);
            cacheLines[i]->frameNumberOut(lineSignals[i].frameNumberOutSignal);
            cacheLines[i]->initializedOut(lineSignals[i].initializedOutSignal);
        }
     SC_THREAD(update);
     sensitive << tag << index << frameNumber << we;


    }

    void update() {
       while(true) {
        wait();
        int idx = index->read().to_uint();
        weSignals[idx].write(we->read());
        frameNumberOut->write(lineSignals[idx].frameNumberOutSignal.read());
        bool tlbIsInitialized = lineSignals[idx].initializedOutSignal.read();
        bool tagNumbersMatch = tag->read() == lineSignals[idx].tagOutSignal.read();
        found->write(tlbIsInitialized && tagNumbersMatch);
       }
    }

    /**
     * @brief Destructor for the TLB module.
     */
    ~TLB() {
        for (int i = 0; i < tlbSize; ++i) {
            delete cacheLines[i];
        }

        delete[] cacheLines;
        delete[] lineSignals;
        delete[] weSignals;
    }
};








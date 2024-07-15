#include "cachelines.hpp"

CACHELINES::CACHELINES(sc_module_name name, unsigned tlbsize) : sc_module(name), tlbsize(tlbsize),
cache(new CACHELINE *[tlbsize]),lineSignals(new LineSignals[tlbsize]){

    for (int i = 0; i < tlbsize; ++i) {

        cache[i] = new CACHELINE(("CacheLine_" + std::to_string(i)).c_str());
        cache[i]->tagIn(tag);
        cache[i]->dataIn(frameNumber);

        cache[i]->we(lineSignals[i].weSignal);
        cache[i]->tagOut(lineSignals[i].tagOutSignal);
        cache[i]->frameNumberOut(lineSignals[i].frameNumberOutSignal);
        cache[i]->initializedOut(lineSignals[i].initializedOutSignal);
    }

    SC_THREAD(update);
    sensitive << tag << index << frameNumber << we;
}

void CACHELINES::update() {
    while (true) {
        wait();
        unsigned int idx = index->read().to_uint();
        lineSignals[idx].weSignal.write(we->read());
        frameNumberOut->write(lineSignals[idx].frameNumberOutSignal.read());
        bool tlbIsInitialized = lineSignals[idx].initializedOutSignal.read();
        bool tagNumbersMatch = tag->read() == lineSignals[idx].tagOutSignal.read();
        found->write(tlbIsInitialized && tagNumbersMatch);
    }
}

CACHELINES::~CACHELINES() {
    for (unsigned i = 0; i < tlbsize; ++i) {
        delete cache[i];
    }

    delete[] cache;
    delete[] lineSignals;
}

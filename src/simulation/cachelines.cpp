#include "cachelines.hpp"

CACHELINES::CACHELINES(sc_module_name name, unsigned tlbsize) : sc_module(name), tlbsize(tlbsize),
cache(new CACHELINE *[tlbsize]),lineSignals(new LineSignals[tlbsize]){

    for (int i = 0; i < tlbsize; ++i) {

        cache[i] = new CACHELINE(("CacheLine_" + std::to_string(i)).c_str());
        cache[i]->tagIn(tag);
        cache[i]->dataIn(frameNumber);

        cache[i]->weIn(lineSignals[i].weSignal);
        cache[i]->tagStore.q(lineSignals[i].tagSignal);
        cache[i]->tagOut(cache[i]->tagStore.q);
        cache[i]->frameNumberStore.q(lineSignals[i].frameNumberSignal);
        cache[i]->frameNumberOut(cache[i]->frameNumberStore.q);
        lineSignals[i].initializedOutSignal.write(true);
    }

    SC_THREAD(update);
    sensitive << tag << index << frameNumber << we;
}

void CACHELINES::update() {
    while (true) {
        wait();
        unsigned int idx = index->read().to_uint();
        lineSignals[idx].weSignal.write(we->read());
        lineSignals[idx].frameNumberSignal.write(frameNumber.read());
        lineSignals[idx].tagSignal.write(tag.read());
        frameNumberOut->write(cache[idx]->frameNumberStore.q->read());
        bool tlbIsInitialized = lineSignals[idx].initializedOutSignal.read();
        bool tagNumbersMatch = tag->read() == cache[idx]->tagStore.q.read();;
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

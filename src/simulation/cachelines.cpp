#include "cachelines.hpp"

CACHELINES::CACHELINES(sc_module_name name, unsigned tlbsize) : sc_module(name), tlbsize(tlbsize),
cache(new CACHELINE *[tlbsize]),lineSignals(new LineSignals[tlbsize]){

    for (int i = 0; i < tlbsize; ++i) {
        //instantiate the cache line
        cache[i] = new CACHELINE(("CacheLine_" + std::to_string(i)).c_str());

        //bind the ports of the cacheline
        cache[i]->tagIn(tag);
        cache[i]->dataIn(frameNumber);
        cache[i]->weIn(lineSignals[i].weSignal);
        cache[i]->tagStore.q(lineSignals[i].tagSignal);
        cache[i]->tagOut(cache[i]->tagStore.q);
        cache[i]->frameNumberStore.q(lineSignals[i].frameNumberSignal);
        cache[i]->frameNumberOut(cache[i]->frameNumberStore.q);

        //set the initialized signal to true
        lineSignals[i].initializedOutSignal.write(true);
    }

    SC_THREAD(update);
    sensitive << tag << index << frameNumber << we;
}

void CACHELINES::update() {
    while (true) {
        wait();
        //acquire the index
        unsigned int idx = index->read().to_uint();

        //update the signals of the selected cache line
        lineSignals[idx].weSignal.write(we->read());
        lineSignals[idx].frameNumberSignal.write(frameNumber.read());
        lineSignals[idx].tagSignal.write(tag.read());

        //adjust the output
        frameNumberOut->write(cache[idx]->frameNumberStore.q->read());

        //set the found signal
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

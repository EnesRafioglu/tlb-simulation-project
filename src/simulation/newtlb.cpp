#include "newtlb.hpp"

NEWTLB::NEWTLB(sc_module_name name, CACHELINES* cache)
    : sc_module(name), cache(cache), tlbsize(cache->tlbsize) {

    cache->tag(tagSignal);
    cache->index(indexSignal);
    cache->we(we);
    cache->frameNumber(frameNumberIn);
    cache->found(found);
    cache->frameNumberOut(frameNumberOut);

    SC_THREAD(update);
    sensitive << pageNumberIn << frameNumberIn << we;
}

void NEWTLB::update() {
    while (true) {
        wait();
        unsigned int numIndex = std::log2(tlbsize);
        unsigned int vp = pageNumberIn->read().to_uint();
        indexSignal.write(takeTrailingNBits(vp, numIndex));
        tagSignal.write(vp >> numIndex);
    }
}


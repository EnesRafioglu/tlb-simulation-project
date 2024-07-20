#include "tlb.hpp"

TLB::TLB(sc_module_name name, CACHELINES* cache) : sc_module(name), cache(cache), tlbsize(cache->tlbsize) {
  //Connect all ports
  cache->tag(tagSignal);
  cache->index(indexSignal);
  cache->we(we);
  cache->frameNumber(frameNumberIn);
  cache->found(found);
  cache->frameNumberOut(frameNumberOut);

  SC_THREAD(update);
  sensitive << pageNumberIn << frameNumberIn << we;
}

void TLB::update() {
  while (true) {
    wait();
    //calculate the amount of bits resrved for the index
    unsigned int numIndex = std::log2(tlbsize);

    //calcute the tag and index of the virtual page and pass them on to the cache
    unsigned int vp = pageNumberIn->read().to_uint();
    indexSignal.write(takeTrailingNBits(vp, numIndex));
    tagSignal.write(vp >> numIndex);
  }
}

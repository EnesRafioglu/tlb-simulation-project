#include "cacheline.hpp"

CACHELINE::CACHELINE(sc_module_name name):
      sc_module(name),
      tagStore("tagStore"),
      frameNumberStore("frameNumberStore")
{
    //Bind the signals
    tagStore.d(tagIn);
    tagStore.we(weIn);

    frameNumberStore.d(dataIn);
    frameNumberStore.we(weIn);

}


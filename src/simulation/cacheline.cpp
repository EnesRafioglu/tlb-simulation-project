#include "cacheline.hpp"

CACHELINE::CACHELINE(sc_module_name name):
      sc_module(name),
      tagStore("tagStore"),
      frameNumberStore("frameNumberStore"),
      initializedStore("initializedStore")
{
    //Bind the signals
    tagStore.d(tagIn);
    tagStore.we(we);
    tagStore.q(tagSignal);
    tagOut(tagSignal);

    frameNumberStore.d(dataIn);
    frameNumberStore.we(we);
    frameNumberStore.q(frameNumberSignal);
    frameNumberOut(frameNumberSignal);

    constantTrue.write(true);

    initializedStore.d(constantTrue);
    initializedStore.we(we);
    initializedStore.q(initializedSignal);
    initializedOut(initializedSignal);
}


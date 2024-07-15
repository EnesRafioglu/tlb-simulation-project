#include "cacheline.hpp"

CACHELINE::CACHELINE(sc_module_name name):
      sc_module(name),
      tagStore("tagStore"),
      frameNumberStore("frameNumberStore"),
      initializedStore("initializedStore")
{
    //Bind the signals
    tagStore.d(tagIn);
    tagStore.we(weIn);


    frameNumberStore.d(dataIn);
    frameNumberStore.we(weIn);



    constantTrue.write(true);

    initializedStore.d(constantTrue);
    initializedStore.we(weIn);
    initializedStore.q(initializedOut);

}


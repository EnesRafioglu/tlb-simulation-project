#include "cache_line.hpp"

CACHELINE::CACHELINE(sc_module_name name) :
  sc_module(name),
  tagStore("tagStore", 0),
  frameNumberStore("frameNumberStore", 0),
  initializedStore("initializedStore", false) 
{
  //Bind the signals
  tagStore.d(tagIn);
  tagStore.we(writeEnabled);
  tagStore.q(tagOut);

  frameNumberStore.d(dataIn);
  frameNumberStore.we(writeEnabled);
  frameNumberStore.q(frameNumberOut);

  constantTrueSignal.write(true);
  initializedStore.d(constantTrueSignal);
  initializedStore.we(writeEnabled);
  initializedStore.q(isInitializedOut);
}

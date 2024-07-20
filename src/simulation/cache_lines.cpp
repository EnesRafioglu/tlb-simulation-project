#include "cache_lines.hpp"

#include "logging.hpp"

CACHELINES::CACHELINES(sc_module_name name, size_t numLines) : sc_module(name) {
  //? For note on the usage of std::unique_ptr, see
  //? src/simulation/cache_lines.hpp

  // Initialize the cache lines
  cacheLines.reserve(numLines);

  for (size_t i = 0; i < numLines; i++) {
    std::unique_ptr<CACHELINE> cacheLine = std::make_unique<CACHELINE>(sc_gen_unique_name("cache_line"));

    std::unique_ptr<CacheLineEntry> cacheLineEntry = std::make_unique<CacheLineEntry>();
    cacheLineEntry->cacheline = std::move(cacheLine);

    cacheLineEntry->cacheline->tagIn(tag);
    cacheLineEntry->cacheline->dataIn(frameNumberIn);

    cacheLineEntry->cacheline->writeEnabled(cacheLineEntry->writeEnabled);
    cacheLineEntry->cacheline->tagOut(cacheLineEntry->tagOut);
    cacheLineEntry->cacheline->frameNumberOut(cacheLineEntry->frameNumberOut);
    cacheLineEntry->cacheline->isInitializedOut(cacheLineEntry->isInitializedOut);

    cacheLineEntry->writeEnabled.write(false);
    cacheLineEntry->isInitializedOut.write(false);

    cacheLines.push_back(std::move(cacheLineEntry));
  }

  // Register the cache lines operation method and sensitivity list
  SC_THREAD(cacheLinesOperation);

  sensitive << tag << index << frameNumberIn << writeEnabled;
}

void CACHELINES::cacheLinesOperation() {
  // Protect against empty cache lines -> access into empty vector
  if (cacheLines.empty()) {
    return;
  }

  while (true) {
    // Reset the write enable signal of the last selected cache line
    cacheLines[lastSelectedIndex]->writeEnabled.write(false);

    // Get the index
    size_t idx = index.read().to_uint();

    // Update the signals of the selected cache line
    //? We decided for this reasonable abstraction to simulate the decoder that
    //? selects the cache line by indexing into the cacheLines vector.
    //?
    //? We still count this as a decoder in the gate count.
    cacheLines[idx]->writeEnabled.write(writeEnabled.read());

    // Wait four delta cycles for the cache line to process the request
    for (int i = 0; i < 4; ++i) {
      wait(SC_ZERO_TIME);
    }

    // Build the response
    //? Same as with the decoder, we simulate a multiplexer and comparator here
    //?
    //? For a pure implementation, look at our reference implementation in
    //? TLB_Design.circ (logism evolution file)
    const bool wasFound = cacheLines[idx]->isInitializedOut.read() && tag.read() == cacheLines[idx]->tagOut.read();
    frameNumberOut.write(cacheLines[idx]->frameNumberOut.read());
    found.write(wasFound);

    debug_log(
      "CacheLines",
      "Mode: " << (writeEnabled.read() ? "Write" : "Read") << ", Index: " << std::dec << idx << std::boolalpha << ", Tags match" << (tag.read() == cacheLines[idx]->tagOut.read()) << ", initialized:" << cacheLines[idx]->isInitializedOut.read() << std::noboolalpha << std::endl
    );

    wait();
  }
}

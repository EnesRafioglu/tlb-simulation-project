#pragma once

#include <stddef.h>

#include "../shared_types.h"

/**
 * @brief Runs the TLB and controller simulation with the given parameters.
 *
 * @param cycles Maximum number of cycles to run the simulation.
 * @param tlbSize Size of the TLB.
 * @param tlbsLatency Latency of the TLB.
 * @param blocksize Size of each block.
 * @param v2bBlockOffset Offset for virtual to block address translation.
 * @param memoryLatency Latency of the memory access.
 * @param numRequests Number of requests to process.
 * @param requests Array of requests to be processed.
 * @param tracefile Tracefile for logging
 * @return Result of the simulation including cycles, misses, hits, and primitive gate count.
 */
extern
//? This allows us to use the same header file in both C and C++ code.
#ifdef LANG_CPP
"C"
#endif
struct Result run_simulation(
  int cycles,
  unsigned tlbSize,
  unsigned tlbsLatency,
  unsigned blocksize,
  unsigned v2bBlockOffset,
  unsigned memoryLatency,
  size_t numRequests,
  struct Request requests [],
  const char* tracefile

);

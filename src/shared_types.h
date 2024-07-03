#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @struct Request
 * @brief Represents the request to be processed by the simulation.
 *
 * This struct contains the address to be accessed, the data to be written
 * or the data that was read, and the write enable flag.
 */
struct Request {
  uint32_t addr;
  uint32_t data;
  int we;
};

/**
 * @struct Response
 * @brief Represents the response to a list of requests processed by the simulation.
 *
 * This struct contains the statistics of the simulation, such as the number of
 * cycles, the number of cache misses, the number of cache hits, and the number
 * of primitive gates used.
 */
struct Result {
  size_t cycles;
  size_t misses;
  size_t hits;
  size_t primitiveGateCount;
};

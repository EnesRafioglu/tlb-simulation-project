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
  /** Address to read from/write to */
  uint32_t addr;
  /** Data to write, or data read after simulation */
  uint32_t data;
  /** Write enable flag (true -> Write, false -> Read) */
  int we;
};

/**
 * @struct Result
 * @brief Represents the response to a list of requests processed by the simulation.
 *
 * This struct contains the statistics of the simulation, such as the number of
 * cycles, the number of cache misses, the number of cache hits, and the number
 * of primitive gates used.
 */
struct Result {
  /** @brief Number of cycles taken to process the requests
   * 
   * Will be `INT_MAX` if the simulation did not finish processing the requests.
  */
  size_t cycles;
  /** Number of TLB cache misses */
  size_t misses;
  /** Number of TLB cache hits */
  size_t hits;
  /** Number of primitive gates required to implement the simulated TLB in hardware */
  size_t primitiveGateCount;
};

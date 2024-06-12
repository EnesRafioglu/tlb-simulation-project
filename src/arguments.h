#pragma once

/**
 * @struct Configuration
 * @brief Represents the configuration parameters for a simulation.
 *
 * This struct contains various parameters that are used to configure a
 * simulation. It includes the number of cycles to simulate, the size of a
 * memory block in bytes, the offset used to "translate" virtual addresses to
 * physical ones, the number of entries in the TLB, the latency of the TLB, the
 * latency of the main memory, the output file for a trace file with all
 * signals, and the input file that contains the data to be processed.
 */
struct Configuration {
  /** Number of cycles to simulate */
  int cycles;
  /** Size of a memory block in byte */
  unsigned int blocksize;
  /** Offset used to "translate" virtual addresses to physical ones */
  unsigned int v2bBlockOffset;
  /** Number of entries in the TLB */
  unsigned int tlbSize;
  /**
   * The latency of the TLB in cycles.
   * Latency is independent of hit or miss.
   * */
  unsigned int tlbLatency;
  /**
   * The latency of the main memory in cycles.
   * Read and write operations have the same latency.
   */
  unsigned int memoryLatency;
  /**
   * Output file for a trace file with all signals.
   * If this is `NULL`, no trace file should be created.
   * */
  const char* traceFile;
  /** The input file that contains the data to be processed. */
  const char* inputFile;
};

/**
 * Parses the command line arguments and populates the Configuration structure.
 * The function sets sensible defaults for all configuration parameters before
 * parsing the command line arguments. If an error occurs during parsing, an
 * error message is printed to stderr and the program is aborted.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @param cnf A pointer to the Configuration structure to be populated.
 */
void parse_arguments(int argc, char* argv[], struct Configuration* cnf);

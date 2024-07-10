#include "main.h"

#include <limits.h>
#include <stdio.h>

#include "arguments.h"
#include "simulation/simulation.hpp"

int main(int argc, char *argv[]) {
  // Check if the architecture is supported
  if (check_architecture() != 0) {
    return 1;
  }

  struct Configuration cnf;
  parse_arguments(argc, argv, &cnf);

  struct Request sampleRequests[] = {{0x00000008U, 0x00000000U, 0},
                                     {0x00000004U, 0x00000011U, 1},
                                     {0x00000012U, 0x00000000U, 0},
                                     {0x00000004U, 0x00000000U, 0},
                                     {0x00000004U, 0x000007d3U, 1}};

  cnf.cycles = 100000;
  cnf.tlbSize = 8;
  cnf.tlbLatency = 2;
  cnf.blocksize = 2;
  cnf.v2bBlockOffset = 4;
  cnf.memoryLatency = 10;
  cnf.traceFile = NULL;

  struct Result result = run_simulation(
      cnf.cycles, cnf.tlbSize, cnf.tlbLatency, cnf.blocksize,
      cnf.v2bBlockOffset, cnf.memoryLatency, 5, sampleRequests, cnf.traceFile);

  printf("Read request returned value: %d (should be 17)\n",
         sampleRequests[3].data);
  printf("Simulation took %zu cycles\n", result.cycles);
  printf("Simulation had %zu misses\n", result.misses);
  printf("Simulation had %zu hits\n", result.hits);

  return 0;
}

int check_architecture() {
  // Check if the size of a byte is 8 bits
  if (CHAR_BIT != 8) {
    fprintf(stderr,
            "Error: Architecture is not supported, the size of one byte is not "
            "8 bits\n");
    return 1;
  }

  // Check if the size of an int is 32 bits
  if (sizeof(int) != 4) {
    fprintf(stderr,
            "Error: Architecture is not supported, the size of an integer is "
            "not 32 bits\n");
    return 1;
  }

  return 0;
}

#include <stdio.h>

#include "arguments.h"
#include "simulation/simulation.hpp"

int main(int argc, char *argv[]) {
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

#include "main.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "arguments.h"
#include "csv.h"
#include "simulation/simulation.hpp"

int main(int argc, char* argv[]) {
  // Check if the architecture is supported
  if (check_architecture() != 0) {
    return 1;
  }

  // Parse the command line arguments
  struct Configuration cnf;
  switch (parse_arguments(argc, argv, &cnf)) {
    case PARSE_ARG_RESULT_ERROR:
      print_usage();

      // 22 is the exit code for invalid arguments
      return 22;
    case PARSE_ARG_RESULT_HELP:
      print_usage();

      // The user only wanted to see the help message -> exit successfully
      return 0;
    default:
      break;
  }

  // Parse the input file
  int line_count;
  struct Request* requests;
  requests = parse_csv(cnf.inputFile, &line_count);

  printf("Number of requests: %u\n", line_count);
  if (requests == NULL) {
    printf("Error: Couldn't parse input file %s\n", cnf.inputFile);
    return 1;
  }

  for (int i = 0; i < line_count; i++) {
    struct Request req;
    req = requests[i];

    printf("Request %d: addr=%u, data=%u, we=%d\n", i, req.addr, req.data,
           req.we);
  }

  // Run the simulation
  struct Result result =
      run_simulation(cnf.cycles, cnf.tlbSize, cnf.tlbLatency, cnf.blocksize,
                     cnf.v2bBlockOffset, cnf.memoryLatency, line_count,
                     requests, cnf.traceFile);

  // Print the results
  printf("Simulation took %zu cycles\n", result.cycles);
  printf("Simulation had %zu misses\n", result.misses);
  printf("Simulation had %zu hits\n", result.hits);
  printf("Primitive gate count: %zu\n", result.primitiveGateCount);

  printf("Read requests:\n");
  for (int i = 0; i < line_count; i++) {
    struct Request req;
    req = requests[i];

    if (req.we == 0) {
      printf("Request %d: addr=%u, data=%u, we=%d\n", i, req.addr, req.data,
             req.we);
    }
  }

  // Clean up
  free(requests);

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

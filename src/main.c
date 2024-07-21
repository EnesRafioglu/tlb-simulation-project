#include "main.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "arguments.h"
#include "csv.h"
#include "simulation/simulation.hpp"

int main(int argc, char* argv[]) {
  int return_code = 0;

  // Check if the architecture is supported
  if (check_architecture() != 0) {
    return 1;
  }

  // Get own name
  char* own_binary_name = "tlb";
  if (argc > 0 && argv[0] != NULL && argv[0][0] != '\0') {
    own_binary_name = argv[0];
  } else {
    fprintf(stderr, "WARN: Couldn't determine own name\n");
  }

  // Parse the command line arguments
  struct Configuration cnf;
  switch (parse_arguments(argc, argv, &cnf)) {
    case PARSE_ARG_RESULT_ERROR:
      print_usage(own_binary_name);

      // 22 is the exit code for invalid arguments
      return 22;
    case PARSE_ARG_RESULT_HELP:
      print_usage(own_binary_name);

      // The user only wanted to see the help message -> exit successfully
      return 0;
    default:
      break;
  }

  // Parse the input file
  size_t line_count;
  struct Request* requests = NULL;
  switch (parse_csv(cnf.inputFile, &line_count, &requests)) {
    case CSV_RESULT_NO_FILE:
      return 2;
    case CSV_RESULT_IS_DIRECTORY:
      return 21;
    case CSV_RESULT_PERMISSION_DENIED:
      return 1;
    case CSV_RESULT_IO_ERROR:
      return 5;
    case CSV_RESULT_PARSE_ERROR:
      return 22;
    default:
      break;
  }

  printf("Number of requests: %lu\n", line_count);

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

  // Optionally save the results to a CSV file
  //? The gotos allow us to add future main code without having to worry about
  //? executing it in case of an error here -> maintainability
  if (cnf.outputFile != NULL) {
    switch (save_requests_to_csv(cnf.outputFile, line_count, requests)) {
      case CSV_RESULT_SUCCESS:
        break;
      case CSV_RESULT_FILE_EXISTS:
        return_code = 17;
        goto main_cleanup_and_exit;
      case CSV_RESULT_IS_DIRECTORY:
        return_code = 21;
        goto main_cleanup_and_exit;
      case CSV_RESULT_PERMISSION_DENIED:
        return_code = 1;
        goto main_cleanup_and_exit;
      case CSV_RESULT_IO_ERROR:
        return_code = 5;
        goto main_cleanup_and_exit;
      default:
        return_code = 1;
        goto main_cleanup_and_exit;
    }
  }

  // Clean up
main_cleanup_and_exit:
  free(requests);

  //? We don't need to free cnf.traceFile or cnf.inputFile because they are
  //? pointers to the argv array and the argv array is managed by the operating
  //? system.

  return return_code;
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

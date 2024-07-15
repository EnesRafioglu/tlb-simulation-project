#include "arguments.h"

#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/**
 * Macro to check the result of an expression and return PARSE_ARG_RESULT_ERROR
 * if the expression is not equal to 0.
 *
 * @param expr The expression to be evaluated.
 * @return PARSE_ARG_RESULT_ERROR if the expression is not equal to 0, otherwise
 * _nothing_.
 */
#define RETURNS_0_OR_ARG_PARSE_ERROR(expr) \
  if ((expr) != 0) {                       \
    return PARSE_ARG_RESULT_ERROR;         \
  }

// Forward declarations

bool maybe_print_requires_value(int opt);
int validate_and_extract_number(char* argName, char* str, void* value,
                                bool isSigned, const void* min, bool pow2);

/**
 * @brief Enumeration of command line options.
 *
 * This enumeration defines the available command line options for the program.
 * Each option is assigned a unique value, which can be used to identify the
 * option. Options that also have a short version are mapped to their short
 * version equivalent.
 */
enum OPTION {
  OPTION_CYCLES = 'c',
  OPTION_BLOCKSIZE = 1,
  OPTION_V2B_BLOCK_OFFSET = 2,
  OPTION_TLB_SIZE = 3,
  OPTION_TLB_LATENCY = 4,
  OPTION_MEMORY_LATENCY = 5,
  OPTION_TRACE_FILE = 6,
  OPTION_HELP = 'h',

  /** Represents the value returned when an unknown option is encountered */
  OPTION_UNKNOWN = '?',
};

/**
 * @brief An array of long-form command line options.
 *
 * This array defines the command line options that can be passed to the
 * program. Each element in the array represents a single option and its
 * associated attributes.
 */
static struct option longArguments[] = {
    {"cycles", required_argument, NULL, OPTION_CYCLES},
    {"blocksize", required_argument, NULL, OPTION_BLOCKSIZE},
    {"v2b-block-offset", required_argument, NULL, OPTION_V2B_BLOCK_OFFSET},
    {"tlb-size", required_argument, NULL, OPTION_TLB_SIZE},
    {"tlb-latency", required_argument, NULL, OPTION_TLB_LATENCY},
    {"memory-latency", required_argument, NULL, OPTION_MEMORY_LATENCY},
    {"tf", required_argument, NULL, OPTION_TRACE_FILE},
    {"help", no_argument, NULL, OPTION_HELP},
};

enum PARSE_ARG_RESULT parse_arguments(int argc, char* argv[],
                                      struct Configuration* cnf) {
  // Set sensible defaults
  cnf->cycles = 1000;
  cnf->blocksize = 64;
  cnf->v2bBlockOffset = 0;
  cnf->tlbSize = 16;
  cnf->tlbLatency = 10;
  cnf->memoryLatency = 100;
  cnf->traceFile = NULL;

  cnf->inputFile = NULL;

  // Common minimum values for use with `validate_and_extract_number`
  const int signedOne = 1;
  const unsigned int unsignedZero = 0;
  const unsigned int unsignedOne = 1;

  // Parse the command line arguments
  int opt;
  opterr = 0;
  while (optind < argc) {
    if ((opt = getopt_long(argc, argv, "c:h", longArguments, NULL)) != -1) {
      switch (opt) {
        case OPTION_CYCLES:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "cycles", optarg, &cnf->cycles, true, &signedOne, false));
          break;
        case OPTION_BLOCKSIZE:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "blocksize", optarg, &cnf->blocksize, false, &unsignedOne, true));
          break;
        case OPTION_V2B_BLOCK_OFFSET:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "v2b-block-offset", optarg, &cnf->v2bBlockOffset, false,
              &unsignedZero, false));
          break;
        case OPTION_TLB_SIZE:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "tlb-size", optarg, &cnf->tlbSize, false, &unsignedOne, true));
          break;
        case OPTION_TLB_LATENCY:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "tlb-latency", optarg, &cnf->tlbLatency, false, &unsignedZero,
              false));
          break;
        case OPTION_MEMORY_LATENCY:
          RETURNS_0_OR_ARG_PARSE_ERROR(validate_and_extract_number(
              "memory-latency", optarg, &cnf->memoryLatency, false,
              &unsignedZero, false));
          break;
        case OPTION_TRACE_FILE:
          if (optarg == NULL || strlen(optarg) == 0) {
            fprintf(stderr, "Error: --tf requires a value\n\n");
            return PARSE_ARG_RESULT_ERROR;
          }

          cnf->traceFile = optarg;
          break;
        case OPTION_HELP:
          return PARSE_ARG_RESULT_HELP;
        case OPTION_UNKNOWN:
          if (!maybe_print_requires_value(optopt)) {
            fprintf(stderr, "Error: Unknown option: -%c\n\n", optopt);
          }

          return PARSE_ARG_RESULT_ERROR;
        default:
          printf("Unknown option: %c\n", opt);
          return PARSE_ARG_RESULT_ERROR;
      }
    } else {
      // Check if the input file has already been set
      if (cnf->inputFile != NULL) {
        fprintf(stderr, "Error: Multiple input files specified\n\n");

        return PARSE_ARG_RESULT_ERROR;
      }

      // Set the input file
      cnf->inputFile = argv[optind];
      optind++;
    }
  }

  // Check if the input file was specified
  if (cnf->inputFile == NULL) {
    fprintf(stderr, "Error: No input file specified\n\n");

    return PARSE_ARG_RESULT_ERROR;
  }

  // Check if the input file is a CSV file
  if (!has_suffix(cnf->inputFile, ".csv")) {
    fprintf(stderr,
            "WARN: Input file does not have a .csv suffix - will continue for "
            "now\n");
  }

  return PARSE_ARG_RESULT_SUCCESS;
}

/**
 * Checks if a given option requires a value and prints an error message if it
 * does.
 *
 * @param opt The option to check.
 * @return true if the option requires a value, false otherwise.
 */
bool maybe_print_requires_value(int opt) {
  switch (opt) {
    case OPTION_CYCLES:
      fprintf(stderr, "Error: --cycles requires a value\n\n");
      break;
    case OPTION_BLOCKSIZE:
      fprintf(stderr, "Error: --blocksize requires a value\n\n");
      break;
    case OPTION_V2B_BLOCK_OFFSET:
      fprintf(stderr, "Error: --v2b-block-offset requires a value\n\n");
      break;
    case OPTION_TLB_SIZE:
      fprintf(stderr, "Error: --tlb-size requires a value\n\n");
      break;
    case OPTION_TLB_LATENCY:
      fprintf(stderr, "Error: --tlb-latency requires a value\n\n");
      break;
    case OPTION_MEMORY_LATENCY:
      fprintf(stderr, "Error: --memory-latency requires a value\n\n");
      break;
    case OPTION_TRACE_FILE:
      fprintf(stderr, "Error: --tf requires a value\n\n");
      break;
    default:
      return false;
  }

  return true;
}

void print_usage() {
  printf(
      "Usage: ./tlb [OPTIONS] INPUT_FILE\n"
      "Options:\n"
      "  --cycles=<int>\t\tNumber of cycles to simulate\n"
      "  --blocksize=<unsigned>\tSize of a memory block in bytes\n"
      "  --v2b-block-offset=<unsigned>\tOffset used to \"translate\" virtual "
      "addresses to physical ones\n"
      "  --tlb-size=<unsigned>\t\tNumber of entries in the TLB\n"
      "  --tlb-latency=<unsigned>\tThe latency of the TLB in cycles\n"
      "  --memory-latency=<unsigned>\tThe latency of the main memory in "
      "cycles\n"
      "  --tf=<path>\t\t\tOutput path for a trace file with all signals\n"
      "  --help\t\t\tPrint this help message\n");
}

/**
 * Validates and extracts a number from a string.
 *
 * This function parses the given string to an integer or unsigned integer,
 * depending on the value of `isSigned`. It then checks if the parsing was
 * successful and if the parsed value is greater than or equal to the minimum
 * value specified by `min`. If any validation fails, an error message is
 * printed and the function returns `1`.
 *
 * @param argName The name of the argument being validated.
 * @param str The string to parse.
 * @param value A pointer to the variable where the parsed value will be stored.
 * @param isSigned A boolean indicating whether the value should be treated as
 *                 signed or unsigned.
 * @param min A pointer to the minimum value that the parsed value must be
 *            greater than or equal to.
 * @param pow2 A boolean indicating whether the value must be a power of 2.
 * @return 0 if the validation was successful, 1 otherwise.
 */
int validate_and_extract_number(char* argName, char* str, void* value,
                                bool isSigned, const void* min, bool pow2) {
  // Parse the string to an integer
  enum PARSE_RESULT result;
  if (isSigned) {
    result = parse_string_to_int(str, value);
  } else {
    result = parse_string_to_uint(str, value);
  }

  // Check if the parsing was successful
  switch (result) {
    case PARSE_SUCCESS:
      break;
    case PARSE_OUT_OF_RANGE:
      if (isSigned) {
        fprintf(stderr,
                "Error: %s is out of range for the interval (%d, %d)\n\n",
                argName, INT_MIN, INT_MAX);
      } else {
        fprintf(stderr,
                "Error: %s is out of range for the interval (%u, %u)\n\n",
                argName, 0, UINT_MAX);
      }
      return 1;
    case PARSE_NO_DIGITS:
      fprintf(stderr, "Error: %s does not contain any digits\n\n", argName);
      return 1;
    case PARSE_CHARACTERS_LEFT:
      fprintf(stderr, "Error: %s contains extra characters\n\n", argName);
      return 1;
    case PARSE_GENERAL_ERROR:
      fprintf(stderr, "Error: %s is not a valid number\n\n", argName);
      return 1;
  }

  // Check if the value must be a power of 2
  if (pow2 && !is_power_of_two(*(unsigned int*)value)) {
    fprintf(stderr, "Error: %s must be a power of 2\n\n", argName);
    return 1;
  }

  // Check if the value is greater than or equal to the minimum
  if (isSigned && *(int*)value < *(int*)min) {
    fprintf(stderr, "Error: %s must be greater than or equal to %d\n\n",
            argName, *(int*)min);
    return 1;
  } else if (!isSigned && *(unsigned int*)value < *(unsigned int*)min) {
    fprintf(stderr, "Error: %s must be greater than or equal to %u\n\n",
            argName, *(unsigned int*)min);
    return 1;
  }

  return 0;
}

#pragma once

#include <stdint.h>

#include "shared_types.h"

/**
 * @brief Enumeration representing the possible results of parsing a CSV file.
 */
enum PARSE_CSV_RESULT {
  /** The CSV file was parsed successfully. */
  PARSE_CSV_RESULT_SUCCESS,
  /** The CSV file does not exist. */
  PARSE_CSV_RESULT_NO_FILE,
  /** The given path is a directory, not a file. */
  PARSE_CSV_RESULT_IS_DIRECTORY,
  /** The CSV file could not be opened because of lacking permissions. */
  PARSE_CSV_RESULT_PERMISSION_DENIED,
  /** An I/O error occurred while trying to read the CSV file. */
  PARSE_CSV_RESULT_IO_ERROR,
  /** An error occurred while parsing the CSV file. */
  PARSE_CSV_RESULT_PARSE_ERROR
};

/**
 * Parses a CSV file and returns an array of Request structures.
 * Every line should contain the write enable flag, the address
 * and if the write enable flag is set, the data to be written.
 * If non valid input is found, an error message is printed to stderr and the
 * program is aborted.
 *
 * @param filename The path to the CSV file to be parsed.
 * @param line_count A pointer to an integer that will store the number of lines
 * in the CSV file.
 * @param requests A pointer to an array of Request structures that will store
 * the parsed data.
 * @return The result of parsing the CSV file.
 */
enum PARSE_CSV_RESULT parse_csv(const char* filename, int* line_count,
                                struct Request** requests);

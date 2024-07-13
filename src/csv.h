#pragma once

#include <stdint.h>

#include "shared_types.h"


/**
 * Parses a CSV file and returns an array of Request structures.
 * Every line should contain the write enable flag, the address
 * and if the write enable flag is set, the data to be written.
 * If non valid input is found, an error message is printed to stderr and the program is aborted.
 * 
 * @param filename The path to the CSV file to be parsed.
 * @param line_count A pointer to an integer that will store the number of lines in the CSV file.
 * @return A pointer to an array of Request structures containing the parsed data.
 */
struct Request* parse_csv(const char* filename, int* line_count);

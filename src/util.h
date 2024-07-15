#pragma once

#include <stdint.h>

/**
 * @brief Enumeration representing the possible parse results.
 *
 * This enumeration defines the possible outcomes of a parsing operation.
 * Each value represents a different parse result.
 */
enum PARSE_RESULT {
  /** Parsing was successful */
  PARSE_SUCCESS,
  /** The parsed value is out of range */
  PARSE_OUT_OF_RANGE,
  /** No digits were found during parsing */
  PARSE_NO_DIGITS,
  /** There are characters left after parsing */
  PARSE_CHARACTERS_LEFT,
  /** A general parsing error occurred */
  PARSE_GENERAL_ERROR,
};

/**
 * @brief Parses a string to an integer.
 *
 * This function takes a C-style string as input and attempts to convert it to
 * an integer. Decimals, octals, and hexadecimals are supported. The base is
 * determined by the prefix of the string. If the conversion is successful, the
 * function returns the parsed integer. If the conversion fails, a corresponding
 * **PARSE_RESULT** value is returned, indicating the reason for the failure.
 *
 * @param str The C-style string to be parsed.
 * @param value A pointer to the integer where the result will be stored.
 * @return The result of the parsing operation.
 */
enum PARSE_RESULT parse_string_to_int(const char* str, int* value);

/**
 * @brief Parses a string to an unsigned integer.
 *
 * This function takes a C-style string as input and attempts to convert it to
 * an unsigned integer. Decimals, octals, and hexadecimals are supported. The
 * base is determined by the prefix of the string. If the conversion is
 * successful, the function returns the parsed unsigned integer. If the
 * conversion fails, a corresponding **PARSE_RESULT** value is returned,
 * indicating the reason for the failure.
 *
 * @param str The C-style string to be parsed.
 * @param value A pointer to the unsigned integer where the result will be
 * stored.
 * @return The result of the parsing operation.
 */
enum PARSE_RESULT parse_string_to_uint(const char* str, unsigned int* value);

/**
* @brief Parses a string to an unsigned integer.
 *
 * This function takes a C-style string as input and attempts to convert it to
 * an unsigned 32-bit integer. Decimals, octals, and hexadecimals are supported.
 * The base is determined by the prefix of the string. If the conversion is
 * successful, the function returns the parsed unsigned integer. If the
 * conversion fails, a corresponding **PARSE_RESULT** value is returned,
 * indicating the reason for the failure.
 *
 * @param str The C-style string to be parsed.
 * @param value A pointer to the unsigned integer where the result will be
 * stored.
 * @return The result of the parsing operation.
 */
enum PARSE_RESULT parse_string_to_uint32(const char* str, uint32_t* value);

#include "util.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Enumeration defining the types of number parsing from string.
 */
enum PARSE_STRING_TO_NUMBER_TYPE {
  /** Represents parsing to an integer */
  PARSE_STRING_TO_NUMBER_TYPE_INT,
  /** Represents parsing to an unsigned integer */
  PARSE_STRING_TO_NUMBER_TYPE_UINT,
  /** Represents parsing to an unsigned 32-bit integer */
  PARSE_STRING_TO_NUMBER_TYPE_UINT32,
};

// While using void* is a bit of a code smell, it is the only way to have a
// single function that can parse multiple integer types.
enum PARSE_RESULT parse_string_to_number(
    const char* str, void* value, const enum PARSE_STRING_TO_NUMBER_TYPE type) {
  char* endptr = NULL;

  // Set errno to 0 to distinguish success/failure after call
  errno = 0;

  // Convert string to unsigned long
  long long parsedValue = strtoll(str, &endptr, 0);

  long long minVal;
  long long maxVal;

  switch (type) {
    case PARSE_STRING_TO_NUMBER_TYPE_INT:
      minVal = INT_MIN;
      maxVal = INT_MAX;
      break;
    case PARSE_STRING_TO_NUMBER_TYPE_UINT:
      minVal = 0;
      maxVal = UINT_MAX;
      break;
    case PARSE_STRING_TO_NUMBER_TYPE_UINT32:
      minVal = 0;
      maxVal = __UINT32_MAX__;
      break;
    default:
      return PARSE_GENERAL_ERROR;
  }

  // Check for errors
  if ((errno == ERANGE &&
       (parsedValue == LONG_MIN || parsedValue == LONG_MAX)) ||
      (errno == 0 && (parsedValue > maxVal || parsedValue < minVal))) {
    return PARSE_OUT_OF_RANGE;
  }

  if (str == endptr) return PARSE_NO_DIGITS;

  if (errno != 0 && parsedValue == 0) return PARSE_GENERAL_ERROR;

  if (*endptr != '\0') return PARSE_CHARACTERS_LEFT;

  // Return the parsed value
  switch (type) {
    case PARSE_STRING_TO_NUMBER_TYPE_INT:
      *(int*)value = parsedValue;
      break;
    case PARSE_STRING_TO_NUMBER_TYPE_UINT:
      *(unsigned int*)value = parsedValue;
      break;
    case PARSE_STRING_TO_NUMBER_TYPE_UINT32:
      *(uint32_t*)value = parsedValue;
      break;
    default:
      return PARSE_GENERAL_ERROR;
  }

  return PARSE_SUCCESS;
}

enum PARSE_RESULT parse_string_to_int(const char* str, int* value) {
  return parse_string_to_number(str, value, PARSE_STRING_TO_NUMBER_TYPE_INT);
}

enum PARSE_RESULT parse_string_to_uint(const char* str, unsigned int* value) {
  return parse_string_to_number(str, value, PARSE_STRING_TO_NUMBER_TYPE_UINT);
}

enum PARSE_RESULT parse_string_to_uint32(const char* str, uint32_t* value) {
  return parse_string_to_number(str, value, PARSE_STRING_TO_NUMBER_TYPE_UINT32);
}

#include "util.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// While using void* is a bit of a code smell, it is the only way to have a
// single function that can parse both signed and unsigned integers.
enum PARSE_RESULT parse_string_to_number(const char* str, void* value,
                                         bool isSigned) {
  char* endptr = NULL;

  // Set errno to 0 to distinguish success/failure after call
  errno = 0;

  // Convert string to unsigned long
  long parsedValue = strtol(str, &endptr, 0);

  // Check for errors
  long maxVal = isSigned ? INT_MAX : UINT_MAX;
  long minVal = isSigned ? INT_MIN : 0;

  if ((errno == ERANGE &&
       (parsedValue == LONG_MIN || parsedValue == LONG_MAX)) ||
      (errno == 0 && (parsedValue > maxVal || parsedValue < minVal))) {
    return PARSE_OUT_OF_RANGE;
  }


  if (str == endptr) return PARSE_NO_DIGITS;

  if (errno != 0 && parsedValue == 0) return PARSE_GENERAL_ERROR;

  if (*endptr != '\0') return PARSE_CHARACTERS_LEFT;

  // Return the parsed value
  if (isSigned) {
    *(int*)value = parsedValue;
  } else {
    *(unsigned int*)value = parsedValue;
  }

  return PARSE_SUCCESS;
}

enum PARSE_RESULT parse_string_to_int(const char* str, int* value) {
  return parse_string_to_number(str, value, true);
}

enum PARSE_RESULT parse_string_to_uint(const char* str, unsigned int* value) {
  return parse_string_to_number(str, value, false);
}

enum PARSE_RESULT parse_string_to_uint32(const char* str, uint32_t* value) {
  char* endptr = NULL;


  // Set errno to 0 to distinguish success/failure after call
  errno = 0;

  // Convert string to unsigned long
  long parsedValue = strtol(str, &endptr, 0);

  // Check for errors
  long maxVal = __UINT32_MAX__;
  long minVal = 0;

  if ((errno == ERANGE &&
       (parsedValue == LONG_MIN || parsedValue == LONG_MAX)) ||
      (errno == 0 && (parsedValue > maxVal || parsedValue < minVal))) {
    return PARSE_OUT_OF_RANGE;
  }

  if (errno != 0 && parsedValue == 0) return PARSE_GENERAL_ERROR;

  if (str == endptr) return PARSE_NO_DIGITS;

  if (*endptr != '\0') return PARSE_CHARACTERS_LEFT;

  // Return the parsed value
  *value = parsedValue;

  return PARSE_SUCCESS;

}

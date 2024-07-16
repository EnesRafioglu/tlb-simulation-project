#include "csv.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"
#include <sys/types.h>

/**
 * Counts the number of non empty lines in a given file from the current
 * position.
 *
 * If this function doesn't return `1`, the file position will not be altered.
 * If it returns `1`, the file position will be undefined.
 *
 * @param file The file to be read.
 * @return 0 on success, 1 on failure.
 */
  int count_lines(FILE* file, unsigned int* lines) {
  // Save the current position in the file
  off_t pos = ftello(file);
  if (pos == -1) {
    fprintf(stderr, "Error: Couldn't get the current position in the file\n");

    return 1;
  }

  // Count the number of lines
  *lines = 0;
  char buffer[35];

  while (fgets(buffer, sizeof(buffer), file)) {
    if (buffer[0] != '\n' && buffer[0] != '\r') {
      (*lines)++;
    }
  }

  // Cleanup
  int was_eof = feof(file);

  if (fseeko(file, pos, SEEK_SET) == -1) {
    fprintf(stderr, "Error: Couldn't reset the position in the file\n");

    return 1;
  }

  return was_eof ? 0 : 1;
}

/**
 * Tokenizes a string into individual tokens.
 * Works similarly to strtok, but doesn't skip empty tokens
 * and the delimiter is always a comma.
 *
 * @param str The string to be tokenized.
 * @return A pointer to the first token in the string.
 */
char* tokenize_string(char* str) {
  // using static buffer to keep the state of the tokenizer
  // between calls
  static char* buffer = NULL;
  char* token;

  if (str) {
    buffer = str;
  }

  if (buffer == NULL) {
    return NULL;
  }

  token = buffer;

  // find the next comma. If there is no comma,
  // buffer will be set to NULL and the next call
  // will return NULL
  buffer = strchr(buffer, ',');

  if (buffer) {
    *buffer = '\0';
    buffer++;
  }

  return token;
}

/**
 * Trims a token by removing leading and trailing quotes, if present.
 *
 * @param token The token to be trimmed.
 * @return A pointer to the trimmed token.
 */
char* trim_token (char* token) {
  if (strlen(token) > 1) {
    if (token[0] == '\"') {
      if (token[strlen(token) - 1] == '\"') {
        token++;
        token[strlen(token) - 1] = '\0';
      } else {
        fprintf(stderr, "Error: Missing closing quote in token %s\n", token);
        return NULL;
      }
    }
  }
  return token;
}

/**
 * Parses a number from a token and stores it in the provided value pointer.
 * Displays error messages with the provided type and line number if parsing
 * fails.
 *
 * @param token The token containing the number to be parsed.
 * @param value A pointer to the variable where the parsed number will be
 *              stored.
 * @param type The type of the number being parsed (for error message display).
 * @param line_number The line number where the parsing is being performed (for
 *                    error message display).
 * @return Returns 0 if the number was successfully parsed, or a non-zero value
 *         if an error occurred.
 */
int parse_number_with_error_messages(const char* token, uint32_t* value,
                                     const char* type, int line_number) {
  enum PARSE_RESULT result = parse_string_to_uint32(token, value);

  switch (result) {
    case PARSE_SUCCESS:
      return 0;
    case PARSE_OUT_OF_RANGE:
      fprintf(stderr, "Error: %s %s is out of range (line: %d)\n", type, token,
              line_number);

      break;
    case PARSE_NO_DIGITS:
      fprintf(stderr, "Error: %s %s doesn't contain any digits (line: %d)\n",
              type, token, line_number);

      break;
    case PARSE_CHARACTERS_LEFT:
      fprintf(stderr, "Error: %s %s contains extra characters (line: %d)\n",
              type, token, line_number);

      break;
    case PARSE_GENERAL_ERROR:
      fprintf(stderr, "Error: Couldn't parse %s %s (line: %d)\n", type, token,
              line_number);

      break;
  }

  // Future proofing: if more cases are added to the enum, this will catch them
  return 1;
}

enum PARSE_CSV_RESULT parse_csv(const char* filename, unsigned int* line_count,
                                struct Request** requests_out) {
  // Do basic checks to see if the file exists and is not a directory
  struct stat st;
  if (stat(filename, &st) == -1) {
    switch (errno) {
      case ENOENT:
        fprintf(stderr, "Error: File %s does'nt exist\n", filename);

        return PARSE_CSV_RESULT_NO_FILE;
      case EACCES:
        fprintf(stderr, "Error: Permission denied for file %s\n", filename);

        return PARSE_CSV_RESULT_PERMISSION_DENIED;
      default:
        fprintf(stderr, "Error: Couldn't open input file %s\n", filename);

        return PARSE_CSV_RESULT_IO_ERROR;
    }
  }

  if (S_ISDIR(st.st_mode)) {
    fprintf(stderr, "Error: %s is a directory, not a file\n", filename);

    return PARSE_CSV_RESULT_IS_DIRECTORY;
  } else if (!S_ISREG(st.st_mode)) {
    fprintf(stderr, "Error: %s is not a regular file\n", filename);

    return PARSE_CSV_RESULT_IO_ERROR;
  }

  // Open the file
  FILE* file = fopen(filename, "r");
  if (!file) {
    // We assume that the file exists, so if it can't be opened, it's an I/O
    // error (don't try to account for races here as it's an error anyway)
    fprintf(stderr, "Error: Couldn't open input file %s\n", filename);

    return PARSE_CSV_RESULT_IO_ERROR;
  }

  // Count the number of lines in the file to allocate the correct amount of
  // memory
  
  if (count_lines(file, line_count)) {
    fclose(file);
    return PARSE_CSV_RESULT_IO_ERROR;
  }

  if (*line_count == 0) {
    return PARSE_CSV_RESULT_SUCCESS;
  }

  // Allocate memory for the requests
  // Don't yet pass the pointer to the caller, as we might encounter an error
  struct Request* parsed_requests =
      (struct Request*)malloc(*line_count * sizeof(struct Request));
  if (!parsed_requests) {
    fprintf(stderr, "Error: Couldn't allocate memory for requests\n");
    fclose(file);

    return PARSE_CSV_RESULT_PARSE_ERROR;
  }

  // longest possible line is 33 characters long
  //  "W","0037777777777","0037777777777"\r\n
  // extra 2 characters for the null terminator
  // and to check if the line is too long
  char buffer[39];
  *line_count = 0;

  while (fgets(buffer, sizeof(buffer), file)) {
    // empty lines aren't allowed in csv files except for the last line
    if (buffer[0] == '\n' || buffer[0] == '\r') {
      fprintf(stderr,
              "Error: Empty lines are not allowed (except for the last line) "
              "(line: %d)\n",
              *line_count + 1);

      goto parse_csv_error_cleanup;
    }

    // check if the line is too long
    if (strlen(buffer) > 37) {
      fprintf(stderr,
              "Error: Too many characters encountered. Could be unnecessary "
              "white spaces (line: %d)\n",
              *line_count + 1);

      goto parse_csv_error_cleanup;
    }

    int token_count = 0;

    // erase newline character at end of line, as this would
    // cause the string to int parsing to fail
    char* new_line = strchr(buffer, '\r');
    if (new_line) {
      *new_line = '\0';
    } else if ((new_line = strchr(buffer, '\n'))) {
      *new_line = '\0';
    }

    // tokenize the string
    char* token = tokenize_string(buffer);
    struct Request request;

    while (token) {
      token = trim_token(token);

      if (token == NULL) {
        goto parse_csv_error_cleanup;
      }

      // check for empty tokens
      if (strlen(token) == 0) {

        // empty token is only allowed for read requests 
        // and only at the end of the line to represent 
        // an empty data field
        if ((tokenize_string(NULL) == NULL) && (request.we == 0)) {
          break;
        }
        
        fprintf(stderr,
                "Error: Multiple commas in a row or a comma at the "
                "beginning of a line are not allowed (line: %d)\n",
                *line_count + 1);

        goto parse_csv_error_cleanup;
      }

      if (token_count == 0) {
        if (strlen(token) == 1) {
          if (token[0] == 'R' || token[0] == 'W' || token[0] == 'r' ||
              token[0] == 'w') {
            request.we = token[0] == 'W' || token[0] == 'w';
          } else {
            fprintf(stderr,
                    "Error: Accesstype must be either W or R not '%c' (line: "
                    "%d)\n",
                    token[0], *line_count + 1);

            goto parse_csv_error_cleanup;
          }

        } else {
          fprintf(stderr,
                  "Error: Accesstype must be a single character with or "
                  "without quotes (line: %d)\n",
                  *line_count + 1);

          goto parse_csv_error_cleanup;
        }

      } else if (token_count == 1) {
        // parse the address and check for errors

        if (parse_number_with_error_messages(token, &request.addr, "Address",
                                             *line_count + 1)) {
          goto parse_csv_error_cleanup;
        }

      } else if (token_count == 2) {
        // check if we is set
        if (request.we == 1) {
          // parse the data and check for errors
          if (parse_number_with_error_messages(token, &request.data, "Data",
                                               *line_count + 1)) {
            goto parse_csv_error_cleanup;
          }

        } else {
          fprintf(stderr, "Error: Read type shouldn't have data (line: %d)\n",
                  *line_count + 1);

          goto parse_csv_error_cleanup;
        }
      } else {
        fprintf(stderr, "Error: Too many tokens in line (line: %d)\n",
                *line_count + 1);

        goto parse_csv_error_cleanup;
      }

      token = tokenize_string(NULL);
      token_count++;
    }

    if (token_count < 2 || ((request.we == 1) && (token_count < 3))) {
      fprintf(stderr, "Error: Too few tokens in line (line: %d)\n",
              *line_count + 1);

      goto parse_csv_error_cleanup;
    }

    parsed_requests[*line_count] = request;
    (*line_count)++;
  }

  *requests_out = parsed_requests;

  fclose(file);
  return PARSE_CSV_RESULT_SUCCESS;

// Clean up in case of an error
parse_csv_error_cleanup:
  fprintf(stderr,
          "\texample inputs: "
          "R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");

  fclose(file);
  free(parsed_requests);

  return PARSE_CSV_RESULT_PARSE_ERROR;
}

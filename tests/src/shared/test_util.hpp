#pragma once

#include <string>

/**
 * Converts an integer value to its hexadecimal representation.
 *
 * @param value The integer value to be converted.
 * @return The hexadecimal representation of the input value as a string.
 */
std::string to_hex(int value);

/**
 * Asserts a condition and prints a message if the condition is false.
 * 
 * Exits the program with status code 1 if the condition is false.
 *
 * @param condition The condition to be checked.
 * @param message The message to be printed if the condition is false.
 */
void assert_with_message(bool condition, std::string message);

/**
 * Builds an expected message string for a given variable name, expected value, and actual value.
 *
 * @param variableName The name of the variable.
 * @param expected The expected value.
 * @param got The actual value.
 * @return The formatted expected message string.
 */
std::string build_expected_msg(std::string variableName, std::string expected, std::string got);

/**
 * Builds an expected message string for a given variable name, expected value, and actual value.
 *
 * @param variableName The name of the variable.
 * @param expected The expected value.
 * @param got The actual value.
 * @return The formatted expected message string.
 */
std::string build_expected_msg(std::string variableName, int expected, int got);

/**
 * Builds an expected message string for a given variable name, expected value, and actual value.
 * 
 * The values are formatted as hexadecimal strings.
 *
 * @param variableName The name of the variable.
 * @param expected The expected value.
 * @param got The actual value.
 * @return The formatted expected message string.
 */
std::string build_expected_msg_hex(std::string variableName, int expected, int got);

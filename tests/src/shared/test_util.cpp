#include "test_util.hpp"

#include <iostream>
#include <sstream>
#include <string>

std::string to_hex(int value) {
  std::stringstream stream;
  stream << std::hex << value;
  return stream.str();
}

void assert_with_message(bool condition, std::string message) {
  if (!condition) {
    std::cerr << message << std::endl;
    exit(1);
  }
}

std::string build_expected_msg(std::string variableName, std::string expected, std::string got) {
  return "[" + variableName + "] " "Expected: " + expected + ", Got: " + got;
}

std::string build_expected_msg(std::string variableName, int expected, int got) {
  return build_expected_msg(variableName, std::to_string(expected), std::to_string(got));
}

std::string build_expected_msg_hex(std::string variableName, int expected, int got) {
  return build_expected_msg(variableName, "0x" + to_hex(expected), "0x" + to_hex(got));
}

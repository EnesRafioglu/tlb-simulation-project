#include <limits>
#include <string>

#include <gtest/gtest.h>

extern "C" {
  #include "util.h"
}

//* parse_string_to_int

TEST(ParseStringToIntTest, BasicDecimal) {
  const char* input = "123";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToIntTest, BasicHex) {
  const char* input = "0x7b";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToIntTest, BasicOctal) {
  const char* input = "0173";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToIntTest, NegativeDecimal) {
  const char* input = "-123";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, -123);
}

TEST(ParseStringToIntTest, NegativeHex) {
  const char* input = "-0x7b";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, -123);
}

TEST(ParseStringToIntTest, NegativeOctal) {
  const char* input = "-0173";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, -123);
}

TEST(ParseStringToIntTest, Zero) {
  const char* input = "0";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 0);
}

TEST(ParseStringToIntTest, LeadingZerosHex) {
  const char* input = "0x007b";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToIntTest, LeadingZerosOctal) {
  const char* input = "0000173";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToIntTest, LeadingZerosNegativeHex) {
  const char* input = "-0x007b";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, -123);
}

TEST(ParseStringToIntTest, LeadingZerosNegativeOctal) {
  const char* input = "-0000173";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, -123);
}

TEST(ParseStringToIntTest, JustInRangeUp) {
  std::string str = std::to_string(std::numeric_limits<int>::max());
  const char* input = str.c_str();
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<int>::max());
}

TEST(ParseStringToIntTest, JustInRangeDown) {
  std::string str = std::to_string(std::numeric_limits<int>::min());
  const char* input = str.c_str();
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<int>::min());
}

// TODO: Add tests for in range hex and octal

TEST(ParseStringToIntTest, CharacterAfterNumberDecimal) {
  const char* input = "123x";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToIntTest, CharacterAfterNumberHex) {
  const char* input = "0x7bx";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToIntTest, CharacterAfterNumberOctal) {
  const char* input = "0173x";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToIntTest, EmptyString) {
  const char* input = "";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToIntTest, NoDigits) {
  const char* input = "abc";
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToIntTest, OutOfRangeUp) {
  std::string str = std::to_string((long long)(std::numeric_limits<int>::max()) + 1);
  const char* input = str.c_str();
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToIntTest, OutOfRangeDown) {
  std::string str = std::to_string((long long)(std::numeric_limits<int>::min()) - 1);
  const char* input = str.c_str();
  int value;
  EXPECT_EQ(parse_string_to_int(input, &value), PARSE_OUT_OF_RANGE);
}


//* parse_string_to_uint

TEST(ParseStringToUnsignedIntTest, BasicDecimal) {
  const char* input = "123";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedIntTest, BasicHex) {
  const char* input = "0x7b";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedIntTest, BasicOctal) {
  const char* input = "0173";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedIntTest, NegativeDecimal) {
  const char* input = "-123";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, NegativeHex) {
  const char* input = "-0x7b";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, NegativeOctal) {
  const char* input = "-0173";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, Zero) {
  const char* input = "0";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 0);
}

TEST(ParseStringToUnsignedIntTest, LeadingZerosHex) {
  const char* input = "0x007b";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedIntTest, LeadingZerosOctal) {
  const char* input = "0000173";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedIntTest, LeadingZerosNegativeHex) {
  const char* input = "-0x007b";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, LeadingZerosNegativeOctal) {
  const char* input = "-0000173";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, JustInRangeUp) {
  std::string str = std::to_string(std::numeric_limits<unsigned int>::max());
  const char* input = str.c_str();
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<unsigned int>::max());
}

TEST(ParseStringToUnsignedIntTest, JustInRangeDown) {
  std::string str = std::to_string(std::numeric_limits<unsigned int>::min());
  const char* input = str.c_str();
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<unsigned int>::min());
}

// TODO: Add tests for in range hex and octal

TEST(ParseStringToUnsignedIntTest, CharacterAfterNumberDecimal) {
  const char* input = "123x";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedIntTest, CharacterAfterNumberHex) {
  const char* input = "0x7bx";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedIntTest, CharacterAfterNumberOctal) {
  const char* input = "0173x";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedIntTest, EmptyString) {
  const char* input = "";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToUnsignedIntTest, NoDigits) {
  const char* input = "abc";
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToUnsignedIntTest, OutOfRangeUp) {
  std::string str = std::to_string((unsigned long long)(std::numeric_limits<unsigned int>::max()) + 1);
  const char* input = str.c_str();
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedIntTest, OutOfRangeDown) {
  std::string str = std::to_string((unsigned long long)(std::numeric_limits<unsigned int>::min()) - 1);
  const char* input = str.c_str();
  unsigned int value;
  EXPECT_EQ(parse_string_to_uint(input, &value), PARSE_OUT_OF_RANGE);
}

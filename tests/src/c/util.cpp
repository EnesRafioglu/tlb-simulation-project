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

//* parse_string_to_uint32

TEST(ParseStringToUnsignedInt32Test, BasicDecimal) {
  const char* input = "123";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedInt32Test, BasicHex) {
  const char* input = "0x7b";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedInt32Test, BasicOctal) {
  const char* input = "0173";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedInt32Test, NegativeDecimal) {
  const char* input = "-123";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, NegativeHex) {
  const char* input = "-0x7b";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, NegativeOctal) {
  const char* input = "-0173";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, Zero) {
  const char* input = "0";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 0);
}

TEST(ParseStringToUnsignedInt32Test, LeadingZerosHex) {
  const char* input = "0x007b";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedInt32Test, LeadingZerosOctal) {
  const char* input = "0000173";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, 123);
}

TEST(ParseStringToUnsignedInt32Test, LeadingZerosNegativeHex) {
  const char* input = "-0x007b";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, LeadingZerosNegativeOctal) {
  const char* input = "-0000173";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, JustInRangeUp) {
  std::string str = std::to_string(std::numeric_limits<uint32_t>::max());
  const char* input = str.c_str();
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<uint32_t>::max());
}

TEST(ParseStringToUnsignedInt32Test, JustInRangeDown) {
  std::string str = std::to_string(std::numeric_limits<uint32_t>::min());
  const char* input = str.c_str();
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_SUCCESS);
  EXPECT_EQ(value, std::numeric_limits<uint32_t>::min());
}

// TODO: Add tests for in range hex and octal

TEST(ParseStringToUnsignedInt32Test, CharacterAfterNumberDecimal) {
  const char* input = "123x";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedInt32Test, CharacterAfterNumberHex) {
  const char* input = "0x7bx";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedInt32Test, CharacterAfterNumberOctal) {
  const char* input = "0173x";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_CHARACTERS_LEFT);
}

TEST(ParseStringToUnsignedInt32Test, EmptyString) {
  const char* input = "";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToUnsignedInt32Test, NoDigits) {
  const char* input = "abc";
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_NO_DIGITS);
}

TEST(ParseStringToUnsignedInt32Test, OutOfRangeUp) {
  std::string str = std::to_string((unsigned long long)(std::numeric_limits<uint32_t>::max()) + 1);
  const char* input = str.c_str();
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

TEST(ParseStringToUnsignedInt32Test, OutOfRangeDown) {
  std::string str = std::to_string((unsigned long long)(std::numeric_limits<uint32_t>::min()) - 1);
  const char* input = str.c_str();
  uint32_t value;
  EXPECT_EQ(parse_string_to_uint32(input, &value), PARSE_OUT_OF_RANGE);
}

//* is_power_of_two

TEST(IsPowerOfTwoTest, Zero) {
  EXPECT_EQ(is_power_of_two(0), 0);
}

TEST(IsPowerOfTwoTest, One) {
  EXPECT_EQ(is_power_of_two(1), 1);
}

TEST(IsPowerOfTwoTest, IsPowerOfTwo) {
  for (int i = 0; i < 32; i++) {
    EXPECT_EQ(is_power_of_two(1 << i), 1);
  }
}

TEST(IsPowerOfTwoTest, NotAPowerOfTwo) {
  EXPECT_EQ(is_power_of_two(3), 0);
  EXPECT_EQ(is_power_of_two(5), 0);
  EXPECT_EQ(is_power_of_two(6), 0);
  EXPECT_EQ(is_power_of_two(7), 0);
  EXPECT_EQ(is_power_of_two(9), 0);
  EXPECT_EQ(is_power_of_two(10), 0);
  EXPECT_EQ(is_power_of_two(11), 0);

  for (int i = 3; i < 32; i++) {
    uint32_t powerOfTwo = 1 << i;
    EXPECT_EQ(is_power_of_two(powerOfTwo-1), 0);
    EXPECT_EQ(is_power_of_two(powerOfTwo+1), 0);
  }
}

//* ilog2

TEST(Ilog2Test, Values) {
  for (int i = 0; i < 32; i++) {
    uint32_t powerOfTwo = 1 << i;
    EXPECT_EQ(ilog2(powerOfTwo), i);
  }
}

//* has_suffix

TEST(HasSuffixTest, Basic) {
  const char* str = "hello.txt";
  const char* suffix = ".txt";
  EXPECT_EQ(has_suffix(str, suffix), 1);
}

TEST(HasSuffixTest, NoSuffix) {
  const char* str = "hello.txt";
  const char* suffix = ".csv";
  EXPECT_EQ(has_suffix(str, suffix), 0);
}

TEST(HasSuffixTest, EmptySuffix) {
  const char* str = "hello.txt";
  const char* suffix = "";
  EXPECT_EQ(has_suffix(str, suffix), 1);
}

TEST(HasSuffixTest, EmptyString) {
  const char* str = "";
  const char* suffix = ".txt";
  EXPECT_EQ(has_suffix(str, suffix), 0);
}

TEST(HasSuffixTest, EmptyStringAndSuffix) {
  const char* str = "";
  const char* suffix = "";
  EXPECT_EQ(has_suffix(str, suffix), 1);
}

TEST(HasSuffixTest, StringToShort) {
  const char* str = "a";
  const char* suffix = ".txt";
  EXPECT_EQ(has_suffix(str, suffix), 0);
}

TEST(HasSuffixTest, Equal) {
  const char* str = ".txt";
  const char* suffix = ".txt";
  EXPECT_EQ(has_suffix(str, suffix), 1);
}

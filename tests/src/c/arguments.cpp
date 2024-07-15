#include <gtest/gtest.h>

extern "C" {
  #include "arguments.h"
}

// Test fixture for parse_arguments
class ParseArgumentsTest : public ::testing::Test {
  protected:
    void SetUp() override {
      // Reset getopt state
      optind = 1;

      // Reset opterr
      opterr = 0;

      // Capture stdout and stderr
      testing::internal::CaptureStdout();
      testing::internal::CaptureStderr();
    }
};

//* Unknown option

TEST_F(ParseArgumentsTest, UnknownOption) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"-z"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(2, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: Unknown option: -z\n\n");
}

TEST_F(ParseArgumentsTest, UnknownOptionWithRealOptions) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=128", (char*)"-z", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(4, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: Unknown option: -z\n\n");
}

//* Help

TEST_F(ParseArgumentsTest, PrintHelpShort) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"-h"};
  EXPECT_EQ(PARSE_ARG_RESULT_HELP, parse_arguments(2, argv, &cnf));

  // Check no output
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, PrintHelpLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--help"};
  EXPECT_EQ(PARSE_ARG_RESULT_HELP, parse_arguments(2, argv, &cnf));

  // Check no output
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, PrintHelpWithInputFile) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--help", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_HELP, parse_arguments(3, argv, &cnf));

  // Check no output
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

//* Input file

TEST_F(ParseArgumentsTest, Empty) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(1, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: No input file specified\n\n");
}

TEST_F(ParseArgumentsTest, OnlyInputFile) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(2, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, NotCSVSuffix) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"file.txt"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(2, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: Input file does not have a .csv suffix - will continue for now\n");
}

//* Cycles

TEST_F(ParseArgumentsTest, ArgCyclesShort) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"-c", (char*)"17", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.cycles, 17);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgCyclesLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--cycles", (char*)"17", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.cycles, 17);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgCyclesLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--cycles=17", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.cycles, 17);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgCyclesZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--cycles=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: cycles must be greater than or equal to 1\n\n");
}

TEST_F(ParseArgumentsTest, ArgCyclesNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--cycles=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: cycles must be greater than or equal to 1\n\n");
}

//* Blocksize

TEST_F(ParseArgumentsTest, ArgBlocksizeLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize", (char*)"128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.blocksize, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.blocksize, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeNotPowerOfTwo) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=127", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize must be a power of 2\n\n");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize is out of range for the interval (0, 4294967295)\n\n");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize must be a power of 2\n\n");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeOne) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.blocksize, 1);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeAboveIntMax) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=2147483648", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.blocksize, 2147483648);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeAboveUIntMax) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=4294967296", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize is out of range for the interval (0, 4294967295)\n\n");
}

//* V2bBlockOffset

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--v2b-block-offset", (char*)"128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.v2bBlockOffset, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--v2b-block-offset=128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.v2bBlockOffset, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--v2b-block-offset=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: v2b-block-offset is out of range for the interval (0, 4294967295)\n\n");
}

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--v2b-block-offset=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.v2bBlockOffset, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetNonPowerOfTwo) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--v2b-block-offset=3", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.v2bBlockOffset, 3);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgV2bBlockOffsetJustInRange) {
  struct Configuration cnf;
  std::string arg = "--v2b-block-offset=" + std::to_string(std::numeric_limits<uint32_t>::max());
  char* argv[] = {(char*)"./sim", (char*)arg.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.v2bBlockOffset, std::numeric_limits<uint32_t>::max());

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

//* TlbSize

TEST_F(ParseArgumentsTest, ArgTlbSizeLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size", (char*)"128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeNotPowerOfTwo) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=127", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: tlb-size must be a power of 2\n\n");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: tlb-size is out of range for the interval (0, 4294967295)\n\n");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: tlb-size must be a power of 2\n\n");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeOne) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 1);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeAboveIntMax) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=2147483648", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 2147483648);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeAboveUIntMax) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=4294967296", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: tlb-size is out of range for the interval (0, 4294967295)\n\n");
}

//* TlbLatency

TEST_F(ParseArgumentsTest, ArgTlbLatencyLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency", (char*)"128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: tlb-latency is out of range for the interval (0, 4294967295)\n\n");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyNonPowerOfTwo) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=3", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 3);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyJustInRange) {
  struct Configuration cnf;
  std::string arg = "--tlb-latency=" + std::to_string(std::numeric_limits<uint32_t>::max());
  char* argv[] = {(char*)"./sim", (char*)arg.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, std::numeric_limits<uint32_t>::max());

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

//* MemoryLatency

TEST_F(ParseArgumentsTest, ArgMemoryLatencyLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency", (char*)"128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgMemoryLatencyLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency=128", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, 128);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgMemoryLatencyNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: memory-latency is out of range for the interval (0, 4294967295)\n\n");
}

TEST_F(ParseArgumentsTest, ArgMemoryLatencyZero) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgMemoryLatencyNonPowerOfTwo) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency=3", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, 3);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgMemoryLatencyJustInRange) {
  struct Configuration cnf;
  std::string arg = "--memory-latency=" + std::to_string(std::numeric_limits<uint32_t>::max());
  char* argv[] = {(char*)"./sim", (char*)arg.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, std::numeric_limits<uint32_t>::max());

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

//* TraceFile

TEST_F(ParseArgumentsTest, ArgTraceFileLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tf", (char*)"file.trace", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.traceFile, "file.trace");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTraceFileLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tf=file.trace", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.traceFile, "file.trace");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTraceFileEmpty) {
  struct Configuration cnf;
  // This also covers calls with `--tf=""` as they will be treated as `--tf=`
  // by the shell
  char* argv[] = {(char*)"./sim", (char*)"--tf=", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: --tf requires a value\n\n");
}

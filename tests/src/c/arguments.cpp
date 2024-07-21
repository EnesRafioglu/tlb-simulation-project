#include <gtest/gtest.h>

extern "C" {
  #include "arguments.h"
}

//? print_usage

TEST(PrintUsageTest, Output) {
  // Capture stdout and stderr
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  print_usage("./my_little_potato");

  // Check output
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(
    output,
    "Usage: ./my_little_potato [OPTIONS] INPUT_FILE\n"
    "Options:\n"
    "  --cycles=<int>\t\tNumber of cycles to simulate (default: INT_MAX - 1)\n"
    "  --blocksize=<unsigned>\tSize of a memory block in bytes (default: 64)\n"
    "  --v2b-block-offset=<unsigned>\tOffset used to \"translate\" virtual addresses to physical ones (default: 1)\n"
    "  --tlb-size=<unsigned>\t\tNumber of entries in the TLB (default: 16)\n"
    "  --tlb-latency=<unsigned>\tThe latency of the TLB in cycles (default: 10)\n"
    "  --memory-latency=<unsigned>\tThe latency of the main memory in cycles (default: 100)\n"
    "  --tf=<path>\t\t\tOutput path for a trace file with all signals (optional)\n"
    "  --output=<path>\t\tOutput path for the simulation read results (optional)\n"
    "  --help\t\t\tPrint this help message\n"
  );
  EXPECT_EQ(error, "");

}

//? parse_arguments

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
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.cycles, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: cycles is set to zero, simulation will not run\n");
}

TEST_F(ParseArgumentsTest, ArgCyclesNegative) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--cycles=-1", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: cycles must be greater than or equal to 0\n\n");
}

TEST_F(ParseArgumentsTest, ArgCyclesINT_MAX) {
  struct Configuration cnf;
  std::string arg = "--cycles=" + std::to_string(std::numeric_limits<int>::max());
  char* argv[] = {(char*)"./sim", (char*)arg.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: cycles may not be INT_MAX as it could lead to an ambiguous Result\n\n");
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
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize must be greater than or equal to 4\n\n");
}

TEST_F(ParseArgumentsTest, ArgBlocksizeAboveIntMax) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-size=0", (char*)"--blocksize=2147483648", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.blocksize, 2147483648);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: TLB size is set to zero, simulation will not use TLB\n");
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

TEST_F(ParseArgumentsTest, ArgBlocksizeSmallerThanWord) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=2", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: blocksize must be greater than or equal to 4\n\n");
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
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: TLB size is set to zero, simulation will not use TLB\n");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeZeroOverridesLatency) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=17", (char*)"--tlb-size=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 0);
  EXPECT_EQ(cnf.tlbLatency, 0);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: TLB size is set to zero, simulation will not use TLB\n");
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
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=4", (char*)"--tlb-size=2147483648", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(4, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: Address format does not add up to 32 bits - please adjust blocksize and tlb-size\n\n");
}

TEST_F(ParseArgumentsTest, ArgTlbSizeHighest) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--blocksize=4", (char*)"--tlb-size=1073741824", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 1073741824);

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
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency", (char*)"2", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 2);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgTlbLatencyLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=2", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbLatency, 2);

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
  std::string arg1 = "--tlb-latency=" + std::to_string(std::numeric_limits<uint32_t>::max());
  std::string arg2 = "--memory-latency=" + std::to_string(std::numeric_limits<uint32_t>::max());
  char* argv[] = {(char*)"./sim", (char*)arg1.data(), (char*)arg2.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

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
  char* argv[] = {(char*)"./sim", (char*)"--tlb-latency=0", (char*)"--memory-latency=0", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

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
  char* argv[] = {(char*)"./sim", (char*)"--memory-latency=13", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.memoryLatency, 13);

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

//* Output

TEST_F(ParseArgumentsTest, ArgOutputFileShort) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"-o", (char*)"out.csv", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.outputFile, "out.csv");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgOutputFileLong) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--output", (char*)"out.csv", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.outputFile, "out.csv");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgOutputFileLongEquals) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--output=out.csv", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.outputFile, "out.csv");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ArgOutputFileNotCSV) {
  struct Configuration cnf;
  char* argv[] = {(char*)"./sim", (char*)"--output=out.txt", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(3, argv, &cnf));

  // Check configuration
  EXPECT_STREQ(cnf.outputFile, "out.txt");

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: Output file does not have a .csv suffix - will continue for now\n");
}

TEST_F(ParseArgumentsTest, ArgOutputFileEmpty) {
  struct Configuration cnf;
  // This also covers calls with `--output=""` as they will be treated as
  // `--output=` by the shell
  char* argv[] = {(char*)"./sim", (char*)"--output=", (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(3, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: --output requires a value\n\n");
}

//* valid address format

TEST_F(ParseArgumentsTest, ValidAddressFormat) {
  struct Configuration cnf;
  std::string argTlbSize = "--tlb-size=" + std::to_string(1<<16);
  std::string argBlockSize = "--blocksize=" + std::to_string(1<<16);
  char* argv[] = {(char*)"./sim", (char*)argBlockSize.data(), (char*)argTlbSize.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 1<<16);
  EXPECT_EQ(cnf.blocksize, 1<<16);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ValidAddressFormat2) {
  struct Configuration cnf;
  std::string argTlbSize = "--tlb-size=" + std::to_string(1<<3);
  std::string argBlockSize = "--blocksize=" + std::to_string(1<<8);
  char* argv[] = {(char*)"./sim", (char*)argBlockSize.data(), (char*)argTlbSize.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 1<<3);
  EXPECT_EQ(cnf.blocksize, 1<<8);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "");
}

TEST_F(ParseArgumentsTest, ValidAddressFormatTLBZero) {
  struct Configuration cnf;
  std::string argTlbSize = "--tlb-size=" + std::to_string(0);
  std::string argBlockSize = "--blocksize=" + std::to_string(1<<8);
  char* argv[] = {(char*)"./sim", (char*)argBlockSize.data(), (char*)argTlbSize.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_SUCCESS, parse_arguments(4, argv, &cnf));

  // Check configuration
  EXPECT_EQ(cnf.tlbSize, 0);
  EXPECT_EQ(cnf.blocksize, 1<<8);

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "WARN: TLB size is set to zero, simulation will not use TLB\n");
}

TEST_F(ParseArgumentsTest, InvalidAddressFormatMoreIndexThanPage) {
  struct Configuration cnf;
  std::string argTlbSize = "--tlb-size=" + std::to_string(1<<16);
  std::string argBlockSize = "--blocksize=" + std::to_string(1<<17);
  char* argv[] = {(char*)"./sim", (char*)argBlockSize.data(), (char*)argTlbSize.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(4, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: Address format does not add up to 32 bits - please adjust blocksize and tlb-size\n\n");
}

TEST_F(ParseArgumentsTest, InvalidAddressFormatMoreIndexThanPage2) {
  struct Configuration cnf;
  std::string argTlbSize = "--tlb-size=" + std::to_string(1<<17);
  std::string argBlockSize = "--blocksize=" + std::to_string(1<<16);
  char* argv[] = {(char*)"./sim", (char*)argBlockSize.data(), (char*)argTlbSize.data(), (char*)"file.csv"};
  EXPECT_EQ(PARSE_ARG_RESULT_ERROR, parse_arguments(4, argv, &cnf));

  // Check error message
  std::string output = testing::internal::GetCapturedStdout();
  std::string error = testing::internal::GetCapturedStderr();
  EXPECT_EQ(output, "");
  EXPECT_EQ(error, "Error: Address format does not add up to 32 bits - please adjust blocksize and tlb-size\n\n");
}

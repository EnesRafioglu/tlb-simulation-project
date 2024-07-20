#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "test_util.hpp"

#include "simulation.hpp"

#define WRITE_R(address, data) Request{address, data, 1}
#define READ_R(address) Request{address, 0, 0}

/**
 * @brief Struct representing the parameters for a simulation test case.
 */
struct SimulationTestParams {
  /** Name of the test case. */
  std::string testName;


  //? The following parameters are the same as the ones in the run_simulation function.
  int cycles;
  unsigned tlbSize;
  unsigned tlbsLatency;
  unsigned blocksize;
  unsigned v2bBlockOffset;
  unsigned memoryLatency;
  size_t numRequests;
  std::vector<Request> requests;
  const char* tracefile;

  /** Flag indicating whether to expect gate count. */
  bool expectGateCount;
  /** Expected result of the simulation. */
  Result expected;

  /** Data expected in requests after simulation
   * 
   * (same order as requests, includes writes)
   */
  std::vector<int> expected_data;

  SimulationTestParams(
    std::string testName,
    int cycles,
    unsigned tlbSize,
    unsigned tlbsLatency,
    unsigned blocksize,
    unsigned v2bBlockOffset,
    unsigned memoryLatency,
    size_t numRequests,
    std::vector<Request> requests,
    const char* tracefile,
    bool expectGateCount,
    Result expected,
    std::vector<int> expected_reads
  ) : testName(testName),
      cycles(cycles),
      tlbSize(tlbSize),
      tlbsLatency(tlbsLatency),
      blocksize(blocksize),
      v2bBlockOffset(v2bBlockOffset),
      memoryLatency(memoryLatency),
      numRequests(numRequests),
      requests(requests),
      tracefile(tracefile),
      expectGateCount(expectGateCount),
      expected(expected),
      expected_data(expected_reads) {}
};

/**
 * @brief Parameterized test fixture for running simulation tests.
 */
class SimulationDeathTest : public testing::TestWithParam<SimulationTestParams> {};

/**
 * Returns the name of the simulation test.
 * 
 * Used to generate test names in the test output for parameterized tests.
 *
 * @param info The test parameter information.
 * @return The name of the simulation test.
 */
std::string get_simulation_test_name(const testing::TestParamInfo<SimulationDeathTest::ParamType>& info) {
  return info.param.testName;
}

/**
 * Runs a simulation test with the given parameters and asserts the results.
 *
 * @param params The parameters for the simulation test.
 */
void run_simulation_test(SimulationTestParams params) {
  // Capture stdout (stderr is captured by death test)
  testing::internal::CaptureStdout();

  // Run the actual simulation
  Result result = run_simulation(
    params.cycles,
    params.tlbSize,
    params.tlbsLatency,
    params.blocksize,
    params.v2bBlockOffset,
    params.memoryLatency,
    params.numRequests,
    // This might seem a little weird, but as long as the vector isn't reallocated,
    // the pointer is valid as a C-style array.
    // (This gurantee is not provided for every container, but it is for std::vector)
    params.numRequests > 0 ? &(params.requests[0]) : nullptr,
    params.tracefile
  );

  // Stop capture and discard stdout
  testing::internal::GetCapturedStdout();

  // Compare results
  assert_with_message(
    result.cycles == params.expected.cycles,
    build_expected_msg("cycles", params.expected.cycles, result.cycles)
  );
  assert_with_message(
    result.misses == params.expected.misses,
    build_expected_msg("misses", params.expected.misses, result.misses)
  );
  assert_with_message(
    result.hits == params.expected.hits,
    build_expected_msg("hits", params.expected.hits, result.hits)
  );

  if (params.expectGateCount) {
    assert_with_message(
      result.primitiveGateCount == params.expected.primitiveGateCount,
      build_expected_msg("primitiveGateCount", params.expected.primitiveGateCount, result.primitiveGateCount)
    );
  }

  for (size_t i = 0; i < params.expected_data.size(); i++) {
    assert_with_message(
      params.requests[i].data == params.expected_data[i],
      build_expected_msg_hex("reads[" + std::to_string(i) + "]", params.expected_data[i], params.requests[i].data)
    );
  }

  // Exit with code 0 if all assertions pass
  exit(0);
}

/**
 * Runs the simulation test multiple times with the given parameters.
 *
 * @param params The parameters for the simulation test.
 */
void run_simulation_multiple_times(SimulationTestParams params) {
  // Capture stdout (stderr is captured by death test)
  testing::internal::CaptureStdout();

  for (int i = 0; i < 2; i++) {
    run_simulation(
      params.cycles,
      params.tlbSize,
      params.tlbsLatency,
      params.blocksize,
      params.v2bBlockOffset,
      params.memoryLatency,
      params.numRequests,
      // This might seem a little weird, but as long as the vector isn't reallocated,
      // the pointer is valid as a C-style array.
      // (This gurantee is not provided for every container, but it is for std::vector)
      params.numRequests > 0 ? &(params.requests[0]) : nullptr,
      params.tracefile
    );
  }
}

TEST(SimulationMultipleRunsTest, CantRunSimulationMultipleTimes) {
  SimulationTestParams params = SimulationTestParams(
      "NoRequests",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      1,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      0,
      // Requests
      {},
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        0,
        // Misses
        0,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {}
    );

  
  EXPECT_EXIT(
    run_simulation_multiple_times(params),
    ::testing::ExitedWithCode(1),
    "Simulation has already been run. Please restart the simulation environment to run another simulation."
  );
}

/**
 * Runs a simulation test with the given parameters and asserts the results.
 * 
 * We need to wrap every test in a separate subprocess to reinitialize the
 * SystemC simulation kernel. Sadly, this is the only way to run multiple tests
 * in the same binary.
 *
 * @param params The parameters for the simulation test.
 */
TEST_P(SimulationDeathTest, RunSimulation) {
  EXPECT_EXIT(run_simulation_test(GetParam()), ::testing::ExitedWithCode(0), "");
}

INSTANTIATE_TEST_SUITE_P(
  BasicSimulations,
  SimulationDeathTest,
  testing::Values(
    SimulationTestParams(
      "NoRequests",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      1,
      // Blocksize
      4,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      0,
      // Requests
      {},
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        0,
        // Misses
        0,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {}
    ),
    SimulationTestParams(
      "SingleWriteNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      4,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      1,
      // Requests
      {
        WRITE_R(0x01, 0x11)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        1,
        // Misses
        1,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11
      }
    ),
    SimulationTestParams(
      "SingleReadNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      4,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      1,
      // Requests
      {
        READ_R(0x01)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        1,
        // Misses
        1,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x0
      }
    ),
    SimulationTestParams(
      "SingleWriteReadNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      4,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      2,
      // Requests
      {
        WRITE_R(0x01, 0x11),
        READ_R(0x01)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        2,
        // Misses
        1,
        // Hits
        1,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11,
        0x11
      }
    ),
    SimulationTestParams(
      "SingleReadWriteNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      4,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      2,
      // Requests
      {
        READ_R(0x01),
        WRITE_R(0x01, 0x11)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        2,
        // Misses
        1,
        // Hits
        1,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x0,
        0x11
      }
    ),
    SimulationTestParams(
      "MultipleWritesNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      14,
      // Requests
      {
        WRITE_R(0x01, 0x1),
        WRITE_R(0x02, 0x2),
        WRITE_R(0x03, 0x3),
        WRITE_R(0x04, 0x4),
        WRITE_R(0x05, 0x5),
        WRITE_R(0x04, 0x6),
        WRITE_R(0x03, 0x7),
        WRITE_R(0x02, 0x8),
        WRITE_R(0x01, 0x9),

        READ_R(0x01),
        READ_R(0x02),
        READ_R(0x03),
        READ_R(0x04),
        READ_R(0x05),
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        14,
        // Misses
        1,
        // Hits
        13,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x1,
        0x2,
        0x3,
        0x4,
        0x5,
        0x6,
        0x7,
        0x8,
        0x9,

        0x9,
        0x8,
        0x7,
        0x6,
        0x5,
      }
    ),
    SimulationTestParams(
      "MultipleReadsNoLatency",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      0,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      0,
      // Number of Requests
      6,
      // Requests
      {
        WRITE_R(0x01, 0x11),

        READ_R(0x01),
        READ_R(0x01),
        READ_R(0x01),
        READ_R(0x01),
        READ_R(0x01),
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        6,
        // Misses
        1,
        // Hits
        5,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11,

        0x11,
        0x11,
        0x11,
        0x11,
        0x11
      }
    )
  ),
  get_simulation_test_name
);

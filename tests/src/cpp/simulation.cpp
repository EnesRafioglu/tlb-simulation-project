#include <vector>
#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include "util.hpp"
#include "test_util.hpp"

#include "simulation.hpp"

#define WRITE_R(address, data) Request{address * 4, data, 1}
#define READ_R(address) Request{address * 4, 0, 0}

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
  std::vector<uint32_t> expected_data;

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
    std::vector<uint32_t> expected_reads
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
      16,
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
      16,
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
      16,
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
      16,
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
        WRITE_R(0x01, 0x1), // Miss -> index: 0
        WRITE_R(0x02, 0x2),
        WRITE_R(0x03, 0x3),
        WRITE_R(0x04, 0x4), // Miss -> index: 1
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
        2,
        // Hits
        12,
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
    ),
    SimulationTestParams(
      "MultipleReadsNoWritesNoLatency",

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
      5,
      // Requests
      {
        READ_R(0x01),
        READ_R(0x02),
        READ_R(0x01),
        READ_R(0x02),
        READ_R(0x01),
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        5,
        // Misses
        1,
        // Hits
        4,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x0,
        0x0,
        0x0,
        0x0,
        0x0
      }
    ),
    SimulationTestParams(
      "JustEnoughCycles",

      // Cycles
      2,
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
      2,
      // Requests
      {
        READ_R(0x01),
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
        0x0,
        0x0
      }
    ),
    SimulationTestParams(
      "NotEnoughCycles",

      // Cycles
      1,
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
      2,
      // Requests
      {
        READ_R(0x01),
        READ_R(0x01)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        std::numeric_limits<size_t>::max(),
        // Misses
        1,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x0,
      }
    ),
    SimulationTestParams(
      "DoesntCountMissIfTLBTimedOutShort",

      // Cycles
      5,
      // TLB Size
      5,
      // TLB Latency
      10,
      // Blocksize
      16,
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
        std::numeric_limits<size_t>::max(),
        // Misses
        0,
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
      "DoesCountMissIfMemoryTimedOutShort",

      // Cycles
      5,
      // TLB Size
      4,
      // TLB Latency
      1,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      10,
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
        std::numeric_limits<size_t>::max(),
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
      "DoesntCountHitIfTLBTimedOutMultiple",

      // Cycles
      11,
      // TLB Size
      4,
      // TLB Latency
      1,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      1,
      // Number of Requests
      4,
      // Requests
      {
        WRITE_R(0x01, 0x11), // 1 + 1 + 1 -> 3
        READ_R(0x01), // 1 + 1 + 1 -> 6
        READ_R(0x01), // 1 + 1 + 1 -> 9
        READ_R(0x01) // 1 + 1 + x -> 11 -> x
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        std::numeric_limits<size_t>::max(),
        // Misses
        1,
        // Hits
        2,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11,
        0x11,
        0x11,
        0x0,
      }
    ),
    SimulationTestParams(
      "DoesCountHitIfMemoryTimedOutMultiple",

      // Cycles
      12,
      // TLB Size
      4,
      // TLB Latency
      1,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      1,
      // Number of Requests
      4,
      // Requests
      {
        WRITE_R(0x01, 0x11), // 0-3 -> 4 cycles (1x for: CPU high clock, TLB latency, Miss->Memory latency, Write->Memory latency)
        READ_R(0x01), // 4-6
        READ_R(0x01), // 7-9
        READ_R(0x01) // 10-x (start at 10, because the last finished at 9 and we need to wait for the next positive edge -> 1 TLB latency -> we executed 12 cycles (0-11) => sc_stop)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        std::numeric_limits<size_t>::max(),
        // Misses
        1,
        // Hits
        3,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11,
        0x11,
        0x11,
        0x00,
      }
    )
  ),
  get_simulation_test_name
);

// //TODO: These tests assume that the TLB has no latency for writes!
INSTANTIATE_TEST_SUITE_P(
  SimulationLatencyTests,
  SimulationDeathTest,
  testing::Values(
    SimulationTestParams(
      "SingleWrite",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      11,
      // Blocksize
      16,
      // V2B Block Offset
      100,
      // Memory Latency
      37,
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
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        1 + (1*37) + (1*11) + (1*(37+0)),
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
      "SingleRead",

      // Cycles
      1000,
      // TLB Size
      4,
      // TLB Latency
      11,
      // Blocksize
      16,
      // V2B Block Offset
      100,
      // Memory Latency
      37,
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
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        1 + (1*37) + (1*11) + (1*(37+0)),
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
      "MultipleReadsNoWrites",

      // Cycles
      1000,
      // TLB Size
      8,
      // TLB Latency
      11,
      // Blocksize
      32,
      // V2B Block Offset
      100,
      // Memory Latency
      37,
      // Number of Requests
      5,
      // Requests
      {
        READ_R(0x01),
        READ_R(0x02),
        READ_R(0x20),
        READ_R(0x22),
        READ_R(0x04),
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        5 + (5*37) + (5*11) + (2*(37+0)),
        // Misses
        2,
        // Hits
        3,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x0,
        0x0,
        0x0,
        0x0,
        0x0
      }
    ),
    SimulationTestParams(
      "MultipleWritesNoReads",

      // Cycles
      1000,
      // TLB Size
      8,
      // TLB Latency
      11,
      // Blocksize
      32,
      // V2B Block Offset
      100,
      // Memory Latency
      37,
      // Number of Requests
      5,
      // Requests
      {
        WRITE_R(0x01, 0x1),
        WRITE_R(0x02, 0x2),
        WRITE_R(0x20, 0x3),
        WRITE_R(0x22, 0x4),
        WRITE_R(0x04, 0x5),
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        5 + (5*37) + (5*11) + (2*(37+0)),
        // Misses
        2,
        // Hits
        3,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x1,
        0x2,
        0x3,
        0x4,
        0x5
      }
    ),
    //* More complex test cases with annotations
    SimulationTestParams(
      "WritesAndReads",

      // Cycles
      100000,
      // TLB Size
      4,
      // TLB Latency
      7,
      // Blocksize
      32,
      // V2B Block Offset
      100,
      // Memory Latency
      587,
      // Number of Requests
      15,
      // Requests
      {
        WRITE_R(0x01, 0x1), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x2), // Miss -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x3), // Miss -> Index: 0, Tag: 1
        READ_R(0x20), // Hit -> Index: 0, Tag: 1

        WRITE_R(0xF0, 0x4), // Miss -> Index: 2, Tag: 7
        READ_R(0x22), // Hit -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0xF1, 0x5), // Hit -> Index: 2, Tag: 7
        WRITE_R(0xF8, 0x6), // Miss -> Index: 3, Tag: 7

        READ_R(0x01), // Hit -> Index: 0, Tag: 0
        READ_R(0x22), // Miss -> Index: 0, Tag: 1
        READ_R(0xF0), // Hit -> Index: 2, Tag: 7
        READ_R(0xF8), // Hit -> Index: 3, Tag: 7
        READ_R(0xF1), // Hit -> Index: 2, Tag: 7
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        15 + (15*587) + (15*7) + (8*(587+0)),
        // Misses
        8,
        // Hits
        7,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x1,
        0x2,

        0x1,
        0x3,
        0x0,

        0x4,
        0x3,

        0x1,
        0x5,
        0x6,

        0x1,
        0x3,
        0x4,
        0x6,
        0x5
      }
    ),
    SimulationTestParams(
      "WritesAndReadsJustEnoughCycles",

      // Cycles
      13621,
      // TLB Size
      4,
      // TLB Latency
      7,
      // Blocksize
      32,
      // V2B Block Offset
      100,
      // Memory Latency
      587,
      // Number of Requests
      15,
      // Requests
      {
        WRITE_R(0x01, 0x1), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x2), // Miss -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x3), // Miss -> Index: 0, Tag: 1
        READ_R(0x20), // Hit -> Index: 0, Tag: 1

        WRITE_R(0xF0, 0x4), // Miss -> Index: 2, Tag: 7
        READ_R(0x22), // Hit -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0xF1, 0x5), // Hit -> Index: 2, Tag: 7
        WRITE_R(0xF8, 0x6), // Miss -> Index: 3, Tag: 7

        READ_R(0x01), // Hit -> Index: 0, Tag: 0
        READ_R(0x22), // Miss -> Index: 0, Tag: 1
        READ_R(0xF0), // Hit -> Index: 2, Tag: 7
        READ_R(0xF8), // Hit -> Index: 3, Tag: 7
        READ_R(0xF1), // Hit -> Index: 2, Tag: 7
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        15 + (15*587) + (15*7) + (8*(587+0)),
        // Misses
        8,
        // Hits
        7,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x1,
        0x2,

        0x1,
        0x3,
        0x0,

        0x4,
        0x3,

        0x1,
        0x5,
        0x6,

        0x1,
        0x3,
        0x4,
        0x6,
        0x5
      }
    ),
    SimulationTestParams(
      "WritesAndReadsJustNotEnoughCycles",

      // Cycles
      13621 - 1,
      // TLB Size
      4,
      // TLB Latency
      7,
      // Blocksize
      32,
      // V2B Block Offset
      100,
      // Memory Latency
      587,
      // Number of Requests
      15,
      // Requests
      {
        WRITE_R(0x01, 0x1), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x2), // Miss -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0x22, 0x3), // Miss -> Index: 0, Tag: 1
        READ_R(0x20), // Hit -> Index: 0, Tag: 1

        WRITE_R(0xF0, 0x4), // Miss -> Index: 2, Tag: 7
        READ_R(0x22), // Hit -> Index: 0, Tag: 1

        READ_R(0x01), // Miss -> Index: 0, Tag: 0
        WRITE_R(0xF1, 0x5), // Hit -> Index: 2, Tag: 7
        WRITE_R(0xF8, 0x6), // Miss -> Index: 3, Tag: 7

        READ_R(0x01), // Hit -> Index: 0, Tag: 0
        READ_R(0x22), // Miss -> Index: 0, Tag: 1
        READ_R(0xF0), // Hit -> Index: 2, Tag: 7
        READ_R(0xF8), // Hit -> Index: 3, Tag: 7
        READ_R(0xF1), // Hit -> Index: 2, Tag: 7
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        std::numeric_limits<size_t>::max(),
        // Misses
        8,
        // Hits
        7,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x1,
        0x2,

        0x1,
        0x3,
        0x0,

        0x4,
        0x3,

        0x1,
        0x5,
        0x6,

        0x1,
        0x3,
        0x4,
        0x6,
      }
    )
  ),
  get_simulation_test_name
);

//* These tests are pseudo-randomly generated
std::vector<SimulationTestParams> generate_random_tests(int numTests) {
  std::vector<SimulationTestParams> tests;
  tests.reserve(numTests);

  std::uniform_int_distribution<> seedDistrib(0, std::numeric_limits<int>::max());
  std::random_device                  rand_dev;
  std::mt19937                        seed_rd(rand_dev());

  std::uniform_int_distribution<> cyclesDistrib(1000, 1000000);
  std::uniform_int_distribution<> numRequestsDistrib(1, 5000);
  std::uniform_int_distribution<> numIndexBitsDistrib(1, 17);
  std::uniform_int_distribution<> latencyDistrib(1, 1000);
  std::uniform_int_distribution<> v2bBlockOffsetDistrib(0, 1000);

  std::uniform_int_distribution<> requestTypeDistrib(0, 1);
  std::uniform_int_distribution<> addressDistrib(0, 0xFFFFFFFF);
  std::uniform_int_distribution<> dataDistrib(0, 0xFFFFFFFF);

  for (int i = 0; i < numTests; ++i) {
    int seed = seedDistrib(seed_rd);
    std::mt19937 rd(seed);

    int numCycles = cyclesDistrib(rd);
    int numIndexBits = numIndexBitsDistrib(rd);

    std::uniform_int_distribution<> numTagBitsDistrib(1, 32 - 2 - numIndexBits);
    int numTagBits = numTagBitsDistrib(rd);

    int numPageBits = numTagBits + numIndexBits;

    assert_with_message(numPageBits <= 32, "The number of page bits is too high.");

    unsigned tlbSize = 1 << numIndexBits;
    unsigned blocksize = 1 << (32 - numPageBits);

    unsigned tlbsLatency = latencyDistrib(rd);
    unsigned memoryLatency = latencyDistrib(rd);

    unsigned v2bBlockOffset = v2bBlockOffsetDistrib(rd);

    size_t numRequests = numRequestsDistrib(rd);
    std::vector<Request> requests;
    std::vector<uint32_t> expectedData;
    requests.reserve(numRequests);

    size_t expectedCycles = 0;
    size_t expectedMisses = 0;
    size_t expectedHits = 0;

    std::map<uint32_t, uint32_t> memory;
    std::map<uint32_t, uint32_t> tlb;

    for (size_t j = 0; j < numRequests; j++) {
      expectedCycles += 1;

      bool isWrite = requestTypeDistrib(rd);
      uint32_t address = addressDistrib(rd);
      uint32_t data = isWrite ? dataDistrib(rd) : 0;

      // Allign the address to 32bits
      address &= 0xFFFFFFFC;

      uint32_t virtualPage = takeLeadingNBits(address, numPageBits);
      uint32_t tlbIndex = takeTrailingNBits(virtualPage, numIndexBits);
      uint32_t tlbTag = virtualPage >> numIndexBits;

      // Check if the address is in TLB
      bool isTLBHit = tlb.find(tlbIndex) != tlb.end() && tlb[tlbIndex] == tlbTag;
      expectedCycles += tlbsLatency;

      // Check if we weren't able to finish the TLB request
      if (expectedCycles >= numCycles) {
        expectedCycles = std::numeric_limits<size_t>::max();
        numRequests = j + 1;

        requests.push_back(isWrite ? WRITE_R(address / 4, data) : READ_R(address / 4));
        expectedData.push_back(data);

        break;
      }

      if (isTLBHit) {
        expectedHits++;
      } else {
        expectedCycles += memoryLatency;
        expectedMisses++;
      }

      // Update TLB
      tlb[tlbIndex] = tlbTag;

      // Calculate the physical address
      uint32_t physicalPage = virtualPage + v2bBlockOffset;
      uint32_t physicalAddress = (physicalPage << (32 - numPageBits)) | (address & (blocksize - 1));

      // Memory access
      expectedCycles += memoryLatency;

      // Check if we weren't able to finish the TLB request
      if (expectedCycles >= numCycles) {
        expectedCycles = std::numeric_limits<size_t>::max();
        numRequests = j + 1;

        requests.push_back(isWrite ? WRITE_R(address / 4, data) : READ_R(address / 4));
        expectedData.push_back(data);

        break;
      }
      if (isWrite) {
        memory[physicalAddress] = data;
      } else {
        data = memory[physicalAddress];
      }

      // Push the request
      requests.push_back(isWrite ? WRITE_R(address / 4, data) : READ_R(address / 4));
      expectedData.push_back(data);
    }

    // std::cout << "NumCycles: " << numCycles << ", Expected Cycles: " << expectedCycles << ", Num requests: " << numRequests << ", num TLB hits: " << expectedHits << ", BlockSize: " << blocksize << ", TLB size: " << tlbSize << ", pb: " << numPageBits << std::endl;

    tests.push_back(SimulationTestParams(
      "RandomTest_" + std::to_string(i) + "_seed__" + std::to_string(seed),

      // Cycles
      numCycles,
      // TLB Size
      tlbSize,
      // TLB Latency
      tlbsLatency,
      // Blocksize
      blocksize,
      // V2B Block Offset
      v2bBlockOffset,
      // Memory Latency
      memoryLatency,

      // Number of Requests
      numRequests,
      // Requests
      requests,
      // Tracefile
      nullptr,

      // Expect Gate Count
      false,
      // Expected Result
      Result{
        // Cycles
        expectedCycles,
        // Misses
        expectedMisses,
        // Hits
        expectedHits,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      expectedData
    ));
  }

  return tests;
}

INSTANTIATE_TEST_SUITE_P(
  SimulationGenerativeTests,
  SimulationDeathTest,
  testing::ValuesIn(generate_random_tests(1000)),
  get_simulation_test_name
);

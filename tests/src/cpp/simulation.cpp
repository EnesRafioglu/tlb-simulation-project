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
  
  friend std::ostream& operator<<(std::ostream& os, const SimulationTestParams& dt) {
    os << "[" << dt.testName << "]"
      << "\n\tCycles: " << dt.cycles 
      << "\n\tTLB Size: " << dt.tlbSize 
      << "\n\tTLB Latency: " << dt.tlbsLatency 
      << "\n\tBlocksize: " << dt.blocksize 
      << "\n\tV2B Block Offset: " << dt.v2bBlockOffset 
      << "\n\tMemory Latency: " << dt.memoryLatency 
      << "\n\tNumber of Requests: " << dt.numRequests 
      << "\n\tExpect Gate Count: " << dt.expectGateCount 
      << "\n\tExpected Result: "
      << "\n\t\tCycles: " << dt.expected.cycles
      << "\n\t\tMisses: " << dt.expected.misses
      << "\n\t\tHits: " << dt.expected.hits
      << "\n\t\tPrimitive Gate Count: " << dt.expected.primitiveGateCount;
      // << "\n\tExpected Reads: " << dt.expected_data;

    return os;
  }
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
    ),
    SimulationTestParams(
      "ZeroCyclesNoRequests",

      // Cycles
      0,
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
      0,
      // Requests
      {},
      // Tracefile
      nullptr,

      // Expect Gate Count
      true,
      // Expected Result
      Result{
        // Cycles
        0,
        // Misses
        0,
        // Hits
        0,
        // Primitive Gate Count
        1568
      },
      // Expected Reads
      {}
    ),
    SimulationTestParams(
      "ZeroCyclesWithRequests",

      // Cycles
      0,
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
        WRITE_R(0x01, 0x11),
        READ_R(0x01),
        READ_R(0x01),
        READ_R(0x01)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      true,
      // Expected Result
      Result{
        // Cycles
        std::numeric_limits<size_t>::max(),
        // Misses
        0,
        // Hits
        0,
        // Primitive Gate Count
        1568
      },
      // Expected Reads
      {
        0x11,
        0x00,
        0x00,
        0x00,
      }
    ),
    SimulationTestParams(
      "ZeroTlbSizeNoRequests",

      // Cycles
      100,
      // TLB Size
      0,
      // TLB Latency
      17,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      1,
      // Number of Requests
      0,
      // Requests
      {},
      // Tracefile
      nullptr,

      // Expect Gate Count
      true,
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
      "ZeroTlbSizeWithRequests",

      // Cycles
      100,
      // TLB Size
      0,
      // TLB Latency
      17,
      // Blocksize
      16,
      // V2B Block Offset
      0,
      // Memory Latency
      5,
      // Number of Requests
      4,
      // Requests
      {
        WRITE_R(0x01, 0x11),
        READ_R(0x01),
        READ_R(0x01),
        READ_R(0x01)
      },
      // Tracefile
      nullptr,

      // Expect Gate Count
      true,
      // Expected Result
      Result{
        // Cycles
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        4 + (4*5) + (4*0) + (4*(5+0)),
        // Misses
        4,
        // Hits
        0,
        // Primitive Gate Count
        0
      },
      // Expected Reads
      {
        0x11,
        0x11,
        0x11,
        0x11,
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

INSTANTIATE_TEST_SUITE_P(
  SimulationEdgeCaseTests,
  SimulationDeathTest,
  testing::Values(
    SimulationTestParams(
      "WriteReadTLBSize1",

      // Cycles
      1000,
      // TLB Size
      1,
      // TLB Latency
      1,
      // Blocksize
      16,
      // V2B Block Offset
      1,
      // Memory Latency
      1,
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
        // numReq + (numReq * memLatency) + (numReq * TLBLatency) + (numTLBMiss * (memLatency + TLBWriteLatency))
        2 + (2*1) + (2*1) + (1*(1+0)),
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
    )
  ),
  get_simulation_test_name
);

//* These tests are pseudo-randomly generated
struct RandomTestDistributions {
  std::uniform_int_distribution<int> cycles;
  std::uniform_int_distribution<int> numIndexBits;
  std::uniform_int_distribution<int> latency;
  std::uniform_int_distribution<int> v2bBlockOffset;
  std::uniform_int_distribution<int> numRequests;
  std::uniform_int_distribution<int> requestType;
  std::uniform_int_distribution<int> address;
  std::uniform_int_distribution<int> data;

  RandomTestDistributions(
    std::uniform_int_distribution<int> cyclesDistrib,
    std::uniform_int_distribution<int> numIndexBitsDistrib,
    std::uniform_int_distribution<int> latencyDistrib,
    std::uniform_int_distribution<int> v2bBlockOffsetDistrib,
    std::uniform_int_distribution<int> numRequestsDistrib,
    std::uniform_int_distribution<int> requestTypeDistrib,
    std::uniform_int_distribution<int> addressDistrib,
    std::uniform_int_distribution<int> dataDistrib
  ) :
    cycles(cyclesDistrib),
    numIndexBits(numIndexBitsDistrib),
    latency(latencyDistrib),
    v2bBlockOffset(v2bBlockOffsetDistrib),
    numRequests(numRequestsDistrib),
    requestType(requestTypeDistrib),
    address(addressDistrib),
    data(dataDistrib)
  {}
};

SimulationTestParams generate_random_test(const std::string& name, int seed, RandomTestDistributions& distributions) {
  // Initialize the random number generator
  std::mt19937 rd(seed);

  // Calculate bit lengths
  int numCycles = distributions.cycles(rd);
  int numIndexBits = distributions.numIndexBits(rd);

  std::uniform_int_distribution<> numTagBitsDistrib(0, 32 - 2 - numIndexBits);
  int numTagBits = numTagBitsDistrib(rd);

  int numPageBits = numTagBits + numIndexBits;

  assert_with_message(numPageBits <= 32, "The number of page bits is too high.");

  // Calculate the block size and TLB size from bit lengths
  unsigned tlbSize = 1 << numIndexBits;
  unsigned blocksize = 1 << (32 - numPageBits);

  // Generate latencies and offsets
  unsigned tlbsLatency = distributions.latency(rd);
  unsigned memoryLatency = distributions.latency(rd);
  unsigned v2bBlockOffset = distributions.v2bBlockOffset(rd);

  // Generate the requests
  size_t numRequests = distributions.numRequests(rd);
  std::vector<Request> requests;
  std::vector<uint32_t> expectedData;
  requests.reserve(numRequests);

  size_t expectedCycles = 0;
  size_t expectedMisses = 0;
  size_t expectedHits = 0;
  bool expectTimeout = false;

  std::map<uint32_t, uint32_t> memory;
  std::map<uint32_t, uint32_t> tlb;

  for (size_t j = 0; j < numRequests; j++) {
    // Generate the basic request data
    bool isWrite = distributions.requestType(rd);
    uint32_t address = distributions.address(rd);
    uint32_t data = isWrite ? distributions.data(rd) : 0;

    // Allign the address to 32bits
    address &= 0xFFFFFFFC;

    // Calculate address parts
    uint32_t virtualPage = takeLeadingNBits(address, numPageBits);
    uint32_t tlbIndex = takeTrailingNBits(virtualPage, numIndexBits);
    uint32_t tlbTag = virtualPage >> numIndexBits;

    // Forwards declaration to be able to jump to the end of the loop
    uint32_t physicalPage;
    uint32_t physicalAddress;

    // Check if the address is in TLB
    bool isTLBHit = tlb.find(tlbIndex) != tlb.end() && tlb[tlbIndex] == tlbTag;
    expectedCycles += tlbsLatency;

    // Check if we weren't able to finish the TLB request
    if (expectedCycles >= numCycles) {
      expectTimeout = true;

      goto generate_random_test_done;
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
    physicalPage = virtualPage + v2bBlockOffset;
    physicalAddress = (physicalPage << (32 - numPageBits)) | (address & (blocksize - 1));

    // Memory access
    expectedCycles += memoryLatency;

    // Check if we weren't able to finish the TLB request
    if (expectedCycles >= numCycles) {
      expectTimeout = true;

      goto generate_random_test_done;
    }

    // Save result of memory access
    if (isWrite) {
      memory[physicalAddress] = data;
    } else {
      data = memory[physicalAddress];
    }

    // Push the request
generate_random_test_done:
    requests.push_back(isWrite ? WRITE_R(address / 4, data) : READ_R(address / 4));
    expectedData.push_back(data);

    // Every request needs at least 1 base cycle (CPU high clock)
    expectedCycles += 1;
  }

  // Check if we weren't able to finish all requests
  if (expectTimeout) {
    expectedCycles = std::numeric_limits<size_t>::max();
  }

  // std::cout << "NumCycles: " << numCycles << ", Expected Cycles: " << expectedCycles << ", Num requests: " << numRequests << ", num TLB hits: " << expectedHits << ", BlockSize: " << blocksize << ", TLB size: " << tlbSize << ", pb: " << numPageBits << std::endl;

  return SimulationTestParams(
    name,

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
  );
}

std::vector<SimulationTestParams> generate_random_tests(const std::string& baseName, int numTests, RandomTestDistributions distributions) {
  std::vector<SimulationTestParams> tests;
  tests.reserve(numTests);

  std::uniform_int_distribution<> seedDistrib(0, std::numeric_limits<int>::max());
  std::random_device rand_dev;
  std::mt19937 seed_rd(rand_dev());

  for (int i = 0; i < numTests; ++i) {
    int seed = seedDistrib(seed_rd);

    std::string name = baseName + "_" + std::to_string(i) + "_seed__" + std::to_string(seed);

    tests.push_back(
      generate_random_test(
        name,
        seed,
        distributions
      )
    );
  }

  return tests;
}

SimulationTestParams generate_specific_random_test(const std::string& baseName, int seed, RandomTestDistributions distributions, bool log = false) {
  std::string name = baseName + "__seed__" + std::to_string(seed);

  SimulationTestParams test = generate_random_test(name, seed, distributions);

  if (log) {
    std::cout << "Specific Random Test: " << test << std::endl;
  }

  return test;
}

INSTANTIATE_TEST_SUITE_P(
  SimulationGenerativeTests,
  SimulationDeathTest,
  testing::ValuesIn(
    generate_random_tests(
      "TotallyRandom",
      150,
      RandomTestDistributions(
        // Cycles
        std::uniform_int_distribution<>(1000, 1000000),
        // Number of Index Bits (TLB Size)
        // Tag bits are dynamically calculated with a distribution over [0, 32 - 2 - numIndexBits]
        std::uniform_int_distribution<>(1, 17),
        // TLB Latency
        std::uniform_int_distribution<>(1, 1000),
        // V2B Block Offset
        std::uniform_int_distribution<>(0, 1000),
        // Number of Requests
        std::uniform_int_distribution<>(1, 5000),
        // Request Type
        std::uniform_int_distribution<>(0, 1),
        // Address
        std::uniform_int_distribution<>(0, 0xFFFFFFFF),
        // Data
        std::uniform_int_distribution<>(0, 0xFFFFFFFF)
      )
    )
  ),
  get_simulation_test_name
);

INSTANTIATE_TEST_SUITE_P(
  SimulationSpecificGenerativeTest,
  SimulationDeathTest,
  testing::Values(
    generate_specific_random_test(
      "SpecificTest",
      17062003,
      RandomTestDistributions(
        // Cycles
        std::uniform_int_distribution<>(1000, 1000000),
        // Number of Index Bits (TLB Size)
        // Tag bits are dynamically calculated with a distribution over [0, 32 - 2 - numIndexBits]
        std::uniform_int_distribution<>(1, 17),
        // TLB Latency
        std::uniform_int_distribution<>(1, 1000),
        // V2B Block Offset
        std::uniform_int_distribution<>(0, 1000),
        // Number of Requests
        std::uniform_int_distribution<>(1, 5000),
        // Request Type
        std::uniform_int_distribution<>(0, 1),
        // Address
        std::uniform_int_distribution<>(0, 0xFFFFFFFF),
        // Data
        std::uniform_int_distribution<>(0, 0xFFFFFFFF)
      ),
      false
    )
  ),
  get_simulation_test_name
);

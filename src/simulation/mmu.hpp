#pragma once

#include <systemc>

#include "tlb.hpp"
#include "memory.hpp"

using namespace sc_core;

/**
 * @brief SystemC module representing the Memory Management Unit (MMU).
 *
 * The MMU is responsible for managing the memory operations in a computer system.
 * It handles address translation, data read/write operations, and communication with the Translation Lookaside Buffer (TLB) and Memory modules.
 * This module contains input and output ports for various signals related to memory operations.
 */
SC_MODULE(MMU) {
public:
  SC_HAS_PROCESS(MMU);

  /** Signal that indicates whether the current value of `dateIn` should be persisted */
  sc_in<bool> writeEnabled;
  /** Current address all operations relate to */
  sc_in<uint32_t> address;
  /** Data that should potentially be written to memory */
  sc_in<uint32_t> dataIn;
  /** Data currently in the memory cell at `address` */
  sc_out<uint32_t> dataOut;

  /** Page number of the virtual address that should be looked up */
  sc_out<uint32_t> tlbVirtualPageNumber;
  /** Possibly found matching physical frame number */
  sc_in<uint32_t> tlbPhysicalFrameNumber;
  /** Signal that indicates whether a matching physical frame was found */
  sc_in<bool> tlbFound;

  /** Singnal that indicated whether the memory module should persist the input data */
  sc_out<bool> memWriteEnabled;
  /** Physical address that the memory should use */
  sc_out<uint32_t> memAddress;
  /** Data that should be written to memory */
  sc_out<uint32_t> memDataIn;
  /** Data that was read from memory */
  sc_in<uint32_t> memDataOut;

  /** Event that signals to the CPU that the current operation is done */
  sc_event mmuDone;

private:
  /** Translation Lookaside Buffer used by the MMU */
  TLB* tlb;

  /** Offset for virtual to block address translation */
  unsigned int v2bBlockOffset;
  /** Latency of the TLB */
  unsigned int tlbsLatency;
  /** Latency of the memory access */
  unsigned int memoryLatency;

  /** Number of leading bits of the virtual address used to translate the
   * virtual memory page to a physical memory frame
   */
  unsigned int numPageBits;

  //? We count the number of hits here and not in the TLB module because the
  //? TLB would not increase the hit count if two consecutive virtual addresses
  //? are the same. This is because of the TLB sensitivity list.

  /** Number of TLB cache hits */
  unsigned int numHits;
  /** Number of TLB cache misses */
  unsigned int numMisses;

  //* Signals used for communication between the MMU and the TLB/Memory modules
  // For documentation on the signals, see the corresponding port declarations above

  sc_signal<uint32_t> sigTlbVirtualPageNumber;
  sc_signal<uint32_t> sigTlbPhysicalFrameNumber;
  sc_signal<bool> sigTlbFound;

  sc_signal<bool> sigMemWriteEnabled;
  sc_signal<uint32_t> sigMemAddress;
  sc_signal<uint32_t> sigMemDataIn;
  sc_signal<uint32_t> sigMemDataOut;

public:
  MMU(
    sc_module_name name,
    TLB* tlb,
    Memory* memory,
    unsigned int blocksize,
    unsigned int v2bBlockOffset,
    unsigned int tlbsLatency,
    unsigned int memoryLatency
  );

  /**
   * @brief Returns the number of hits in the TLB.
   *
   * @return The number of hits in the TLB.
   */
  unsigned int getNumHits() const;

  /**
   * @brief Returns the number of cache misses.
   *
   * @return The number of cache misses.
   */
  unsigned int getNumMisses() const;
private:
  /** @brief Method that is called on the positive edge of the clock signal.
   * 
   * This method is responsible for writing values into memory when `writeEnabled` is true.
   * It also reads values from memory and sets `dataOut` to the value stored at `address`.
   */
  void mmuOperation();
};

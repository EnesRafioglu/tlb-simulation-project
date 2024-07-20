#include "mmu.hpp"

#include "util.hpp"
#include "logging.hpp"

MMU::MMU(
  sc_module_name name,
  TLB* tlb,
  Memory* memory,
  unsigned int blocksize,
  unsigned int v2bBlockOffset,
  unsigned int tlbsLatency,
  unsigned int memoryLatency
) : sc_module(name), tlb(tlb), v2bBlockOffset(v2bBlockOffset), tlbsLatency(tlbsLatency), memoryLatency(memoryLatency), numHits(0), numMisses(0) {
  // Calculate the number of bits used for the page number
  // We can simplify this calculation because both virtual and physical addresses are 32 bits
  numPageBits = 32 - std::log2(blocksize);

  // Wire up the tlb module
  tlbVirtualPageNumberIn(sigTlbVirtualPageNumberIn);
  tlbPhysicalFrameNumberOut(sigTlbPhysicalFrameNumberOut);
  tlbFound(sigTlbFound);

  tlb->pageNumberIn(sigTlbVirtualPageNumberIn);
  tlb->frameNumberIn(sigTlbPhysicalFrameNumberIn);
  tlb->writeEnabled(sigTlbWriteEnabled);
  tlb->found(sigTlbFound);
  tlb->frameNumberOut(sigTlbPhysicalFrameNumberOut);

  // Wire up the memory module
  memWriteEnabled(sigMemWriteEnabled);
  memAddress(sigMemAddress);
  memDataIn(sigMemDataIn);
  memDataOut(sigMemDataOut);

  memory->writeEnabled(sigMemWriteEnabled);
  memory->address(sigMemAddress);
  memory->dataIn(sigMemDataIn);
  memory->dataOut(sigMemDataOut);

  // Register the MMU operation method and sensitivity list
  SC_THREAD(mmuOperation);

  dont_initialize();

  sensitive << writeEnabled << address << dataIn;
}

size_t MMU::getNumHits() const {
  return numHits;
}

size_t MMU::getNumMisses() const {
  return numMisses;
}

void MMU::mmuOperation() {
  wait(SC_ZERO_TIME);

  while(true) {
    // Read the address from the CPU
    const uint32_t virtualAddress = address.read().to_uint();

    // Extract the virtual page number from the virtual address
    const uint32_t virtualPageNumber = takeLeadingNBits(virtualAddress, numPageBits);
    debug_log("MMU", "Virtual address: 0x" << std::hex << virtualAddress << " -> virtual page number: 0x" << virtualPageNumber << std::dec);

    // Send the virtual address page to the TLB and wait for the TLB to find the physical frame
    tlbVirtualPageNumberIn.write(virtualPageNumber);

    // Wait delta cycles for the real tlb latency
    debug_log("MMU", "Waiting for actual TLB latency at cycle: " << getNumCyclesRan() << ", delta: " << sc_delta_count());
    wait(sigTlbFound.default_event());
    debug_log("MMU", "End actual TLB latency at cycle: " << getNumCyclesRan() << ", delta: " << sc_delta_count());

    // Wait for tlb latency affecting the cycle count
    debug_log("MMU", "Start TLB latency at cycle: " << getNumCyclesRan());
    wait(tlbsLatency, SC_NS);
    debug_log("MMU", "End TLB latency at cycle: " << getNumCyclesRan());

    // Check if the TLB found the physical frame
    const bool found = tlbFound.read();
    uint32_t physicalFrame = tlbPhysicalFrameNumberOut.read().to_uint();

    if (!found) {
      debug_log("MMU", "TLB miss");
      numMisses++;

      // If the TLB didn't find the physical frame, we need to calculate it
      physicalFrame = virtualPageNumber + v2bBlockOffset;

      // Calculating the physical address would normally require a memory access
      // We simulate this by waiting for the memory latency
      wait(memoryLatency, SC_NS);

      // Store the translation in the TLB for future use
      sigTlbVirtualPageNumberIn.write(virtualPageNumber);
      sigTlbPhysicalFrameNumberIn.write(physicalFrame);
      sigTlbWriteEnabled.write(true);

      // Wait delta cycles for the real tlb latency
      wait(sigTlbFound.default_event());

      sigTlbWriteEnabled.write(false);
    } else {
      debug_log("MMU", "TLB hit");
      numHits++;
    }
    debug_log("MMU", "Physical frame: 0x" << std::hex << physicalFrame << std::dec);

    // Calculate the physical address
    const uint32_t physicalAddress = (physicalFrame << (32 - numPageBits)) | takeTrailingNBits(virtualAddress, (32 - numPageBits));

    debug_log(
      "MMU",
      (found ? "TLB hit" : "TLB miss") << " for virtual address 0x" << std::hex << virtualAddress << " -> physical address 0x" << physicalAddress << std::dec
    );

    // Send the physical address etc. to the memory module and wait for the memory
    bool isWrite = writeEnabled.read();
    memAddress.write(physicalAddress);
    memWriteEnabled.write(isWrite);
    memDataIn.write(dataIn.read());

    // Wait delta cycles for the real memory latency
    // (normally two delta cycles as the memory module is triggered in the next delta cycle
    //  and it's output becomes available in the deltacycle after that)
    debug_log("MMU", "Waiting for actual memory latency at cycle: " << getNumCyclesRan() << ", delta: " << sc_delta_count());
    wait(sigMemDataOut.default_event());
    debug_log("MMU", "End actual memory latency at cycle: " << getNumCyclesRan() << ", delta: " << sc_delta_count());

    // Wait for memory latency affecting the cycle count
    debug_log("MMU", "Start memory latency at cycle: " << getNumCyclesRan());
    wait(memoryLatency, SC_NS);
    debug_log("MMU", "End memory latency at cycle: " << getNumCyclesRan());

    // Read the data from the memory module and send it to the CPU
    dataOut.write(memDataOut.read());

    debug_log(
      "MMU",
      (isWrite ? "Write" : "Read") << " operation at address 0x" << std::hex << virtualAddress << " -> 0x" << physicalAddress << " with data 0x" << (isWrite ? dataIn.read().to_uint() : memDataOut.read()) << std::dec
    );

    // Signal the CPU that the operation is done and wait for the next operation
    mmuDone.notify(SC_ZERO_TIME);
    wait();
  }
}

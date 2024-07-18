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
  tlbVirtualPageNumber(sigTlbVirtualPageNumber);
  tlbPhysicalFrameNumber(sigTlbPhysicalFrameNumber);
  tlbFound(sigTlbFound);

  tlb->virtualPageNumber(sigTlbVirtualPageNumber);
  tlb->physicalFrameNumber(sigTlbPhysicalFrameNumber);
  tlb->found(sigTlbFound);

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

  sensitive << address << writeEnabled << dataIn;
}

unsigned int MMU::getNumHits() const {
  return numHits;
}

unsigned int MMU::getNumMisses() const {
  return numMisses;
}

void MMU::mmuOperation() {
  wait(SC_ZERO_TIME);

  while(true) {
    // Read the address from the CPU
    const uint32_t virtualAddress = address.read().to_uint();

    // Extract the virtual page number from the virtual address
    const uint32_t virtualPageNumber = takeLeadingNBits(virtualAddress, numPageBits);

    // Send the virtual address page to the TLB and wait for the TLB to find the physical frame
    tlbVirtualPageNumber.write(virtualPageNumber);
    wait(tlbsLatency, SC_NS);

    // Check if the TLB found the physical frame
    const bool found = tlbFound.read();
    uint32_t physicalFrame = tlbPhysicalFrameNumber.read().to_uint();

    if (!found) {
      numMisses++;

      // If the TLB didn't find the physical frame, we need to calculate it
      physicalFrame = virtualPageNumber + v2bBlockOffset;

      // Calculating the physical address would normally require a memory access
      // We simulate this by waiting for the memory latency
      wait(memoryLatency, SC_NS);

      // Store the translation in the TLB for future use
      tlb->store(virtualPageNumber, physicalFrame);
    } else {
      numHits++;
    }

    // Calculate the physical address
    const uint32_t physicalAddress = (physicalFrame << (32 - numPageBits)) | takeTrailingNBits(virtualAddress, (32 - numPageBits));

    debug_log(
      "MMU",
      (found ? "TLB hit" : "TLB miss") << " for virtual address 0x" << std::hex << virtualAddress << " -> physical address 0x" << physicalAddress
    );

    // Send the physical address etc. to the memory module and wait for the memory
    memAddress.write(physicalAddress);
    memWriteEnabled.write(writeEnabled.read());
    memDataIn.write(dataIn.read());

    // Wait for memory latency affecting the cycle count
    wait(memoryLatency, SC_NS);
    // Wait delta cycles for the real memory latency
    // (normally two delta cycles as the memory module is triggered in the next delta cycle
    //  and it's output becomes available in the deltacycle after that)
    wait(sigMemDataOut.default_event());

    // TODO: Decide which method of synchronization to use
    // If the memory latency is 0, we need to wait for two delta cycles for
    // the memory module to finish the operation (this doesn't affect the cycle count)
    // if (memoryLatency == 0) {
    //   wait(SC_ZERO_TIME);
    //   wait(SC_ZERO_TIME);
    // }

    // Read the data from the memory module and send it to the CPU
    dataOut.write(memDataOut.read());

    // Signal the CPU that the operation is done and wait for the next operation
    mmuDone.notify(SC_ZERO_TIME);
    wait();
  }
}

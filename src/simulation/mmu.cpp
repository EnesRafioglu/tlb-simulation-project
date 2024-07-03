#include "mmu.hpp"

MMU::MMU(
  sc_module_name name,
  TLB* tlb,
  Memory* memory,
  unsigned int tlbsLatency,
  unsigned int memoryLatency
) : sc_module(name), tlb(tlb), tlbsLatency(tlbsLatency), memoryLatency(memoryLatency) {
  // Wire up the tlb module
  tlbVirtualAddress(sigTlbVirtualAddress);
  tlbPhysicalAddress(sigTlbPhysicalAddress);
  tlbFound(sigTlbFound);

  tlb->virtualAddress(sigTlbVirtualAddress);
  tlb->physicalAddress(sigTlbPhysicalAddress);
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

void MMU::mmuOperation() {
  wait(SC_ZERO_TIME);

  while(true) {
    // Read the address from the CPU
    uint32_t virtualAddress = address.read();

    // Send the virtual address to the TLB and wait for the TLB to find the physical address
    tlbVirtualAddress.write(virtualAddress);
    wait(tlbsLatency, SC_NS);

    // Check if the TLB found the physical address
    bool found = tlbFound.read();
    uint32_t physicalAddress = tlbPhysicalAddress.read();

    if (!found) {
      // If the TLB didn't find the physical address, we need to calculate it
      physicalAddress = tlb->store(virtualAddress);

      // Calculating the physical address would normally require a memory access
      // We simulate this by waiting for the memory latency
      wait(memoryLatency, SC_NS);
    }

    #ifdef DEBUG
    std::cout << "MMU: " << (found ? "TLB hit" : "TLB miss") << " for virtual address 0x" << std::hex << virtualAddress << " -> physical address 0x" << physicalAddress << std::endl;
    #endif

    // Send the physical address etc. to the memory module and wait for the memory
    memAddress.write(physicalAddress);
    memWriteEnabled.write(writeEnabled.read());
    memDataIn.write(dataIn.read());
    wait(memoryLatency, SC_NS);

    // Read the data from the memory module and send it to the CPU
    dataOut.write(memDataOut.read());

    // Signal the CPU that the operation is done and wait for the next operation
    mmuDone.notify(SC_ZERO_TIME);
    wait();
  }
}

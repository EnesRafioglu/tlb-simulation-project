#include "cpu.hpp"

CPU::CPU(
  sc_module_name name,
  MMU* mmu,
  unsigned int numRequests,
  Request* requests
) : sc_module(name), mmu(mmu), numRequests(numRequests), requests(requests), requestIndex(0) {
  // Wire up the mmu module
  writeEnabled(sigWriteEnabled);
  address(sigAddress);
  dataIn(sigDataIn);
  dataOut(sigDataOut);

  mmu->writeEnabled(sigWriteEnabled);
  mmu->address(sigAddress);
  mmu->dataIn(sigDataIn);
  mmu->dataOut(sigDataOut);

  // Register the CPU operation method and sensitivity list
  SC_THREAD(cpuOperation);

  sensitive << clk.pos();
}

void CPU::cpuOperation() {
  while(true) {
    // Stop if all requests have been processed
    if (requestIndex >= numRequests) {
      sc_stop();
      return;
    }

    // Get the current request
    Request* request = &requests[requestIndex];

    #ifdef DEBUG
    std::cout << std::string(25, '+') << std::endl;
    std::cout << "CPU: Processing request " << requestIndex << " with address 0x" << std::hex << request->addr << " and data 0x" << request->data << std::endl;
    #endif

    // Send the request to the MMU
    address.write(request->addr);
    dataIn.write(request->data);
    writeEnabled.write(request->we);

    // Wait for the MMU to process the request
    wait(mmu->mmuDone);

    // If the request was a read, get the data from the MMU
    if (request->we == false) {
      request->data = dataOut.read();

      #ifdef DEBUG
      std::cout << "CPU: Received data 0x" << std::hex << request->data << std::endl;
      #endif
    }

    #ifdef DEBUG
    std::cout << std::string(25, '-') << std::endl;
    #endif

    ++requestIndex;
    wait();
  }
}

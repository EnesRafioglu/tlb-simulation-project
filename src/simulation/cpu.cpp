#include "cpu.hpp"

#include "logging.hpp"

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
  mmu->nextRequest(sigNext);

  sendingNextRequest(sigNext);

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
    Request* const request = &requests[requestIndex];



    // Send the request to the MMU
    address.write(request->addr);
    dataIn.write(request->data);
    writeEnabled.write(request->we);
    //Trigger the MMU
    sigNext.write(!sigNext.read());

    // Wait for the MMU to process the request
    wait(mmu->mmuDone);

    // If the request was a read, get the data from the MMU
    if (request->we == false) {
      request->data = dataOut.read().to_uint();

    }

    ++requestIndex;
    wait();
  }
}

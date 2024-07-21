#include <systemc>
#include <iostream>

#include <systemc>

using namespace sc_core;

extern "C" int test_function();
int test_function() {

  sc_start(0, SC_NS);
  sc_stop();

  return 17062003;
}

// Leave this function as-is, it is required by the SystemC library but will not be used.
int sc_main(int argc, char* argv[]) {
  std::cerr << "Running the SystemC entrypoint function `sc_main` directly is not supported." << std::endl;

  return 1;
}

#include "util.hpp"

#include <systemc>

uint32_t takeLeadingNBits(const uint32_t value, const unsigned int n) {
  return value >> (32 - n);
}

uint32_t takeTrailingNBits(const uint32_t value, const unsigned int n) {
  return value & ((1 << n) - 1);
}

size_t getNumCyclesRan() {
  return sc_core::sc_time_stamp().value() / 1000;
}

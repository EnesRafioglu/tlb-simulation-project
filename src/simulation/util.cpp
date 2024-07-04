#include "util.hpp"

uint32_t takeLeadingNBits(uint32_t value, unsigned int n) {
  return value >> (32 - n);
}

uint32_t takeTrailingNBits(uint32_t value, unsigned int n) {
  return value & ((1 << n) - 1);
}

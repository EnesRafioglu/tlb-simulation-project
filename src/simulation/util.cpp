#include "util.hpp"

uint32_t takeLeadingNBits(const uint32_t value, const unsigned int n) {
  return value >> (32 - n);
}

uint32_t takeTrailingNBits(const uint32_t value, const unsigned int n) {
  return value & ((1 << n) - 1);
}

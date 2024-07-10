#pragma once

#include <cstdint>

/**
 * Takes the leading N bits from the given value.
 *
 * @param value The input value from which to take the leading bits.
 * @param n The number of leading bits to take.
 * @return The resulting value with only the leading N bits.
 */
uint32_t takeLeadingNBits(const uint32_t value, const unsigned int n);

/**
 * Takes the trailing N bits from the given value.
 *
 * @param value The value from which to take the trailing bits.
 * @param n The number of trailing bits to take.
 * @return The resulting value with only the trailing N bits.
 */
uint32_t takeTrailingNBits(const uint32_t value, const unsigned int n);

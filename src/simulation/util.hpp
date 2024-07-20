#pragma once

#include <cstdint>
#include <cstddef>

/**
 * Takes the leading (left) N bits from the given value.
 *
 * @param value The input value from which to take the leading bits.
 * @param n The number of leading bits to take.
 * @return The resulting value with only the leading N bits.
 */
uint32_t takeLeadingNBits(const uint32_t value, const unsigned int n);

/**
 * Takes the trailing (right) N bits from the given value.
 *
 * @param value The value from which to take the trailing bits.
 * @param n The number of trailing bits to take.
 * @return The resulting value with only the trailing N bits.
 */
uint32_t takeTrailingNBits(const uint32_t value, const unsigned int n);

/**
 * @brief Get the number of cycles that fully ran.
 * 
 * It will **not return the number of the current cycle**, but the number of
 * cycles that have been fully executed (e.g. when the simulation just started,
 * it will return 0).
 * 
 * @return The number of cycles ran.
 */
size_t getNumCyclesRan();

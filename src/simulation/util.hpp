#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <sstream>
#include <ostream>
#include <streambuf>

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

/**
 * @brief Helper class to capture output from a stream.
 * 
 * This class can be used to capture the output from a stream and retrieve it
 * later. It is useful for testing purposes.
 * 
 * This class is not thread-safe!
 */
class CaptureStream {
private:
  /** Reference to the stream to capture */
  std::ostream& stream;

  /** Buffer to hold the captured data */
  std::stringstream buffer;

  /** Original buffer we redirected from */
  std::streambuf* originalBuffer;

  /** Indicates whether we are currently capturing data */
  bool isCapturing;
public:

  /**
   * @brief Start capturing from `stream`.
   * 
   * The redirection of the stream will be reverted when the object is
   * destroyed.
   * 
   * @param stream The output stream to capture.
   */
  CaptureStream(std::ostream& stream);

  /**
   * @brief Optionally start capturing `stream`.
   * 
   * The redirection of the stream will be reverted when the object is
   * destroyed.
   * 
   * @param stream The output stream to capture.
   */
  CaptureStream(std::ostream& stream, bool startCapturing);

  /**
   * @brief Copy constructor is deleted.
   */
  CaptureStream(const CaptureStream&) = delete;


  /**
   * @brief Stop capturing the stream.
   * 
   * This allows `CaptureStream` to be used in a RAII manner, just like a lock
   * guard.
   */
  ~CaptureStream();

  /**
   * @brief Start capturing the stream.
   * 
   * Safe to call multiple times.
   */
  void start();

  /**
   * @brief Stop capturing the stream.
   * 
   * Safe to call multiple times.
   */
  void stop();

  /**
   * @brief Get the current contents of the captured buffer.
   * 
   * @return The captured data.
   */
  std::string getCaptured() const;

  /**
   * @brief Check if the stream is currently capturing.
   * 
   * @return `true` if the stream is capturing, `false` otherwise.
   */
  bool isActive() const;
};

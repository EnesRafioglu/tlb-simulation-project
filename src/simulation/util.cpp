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

CaptureStream::CaptureStream(std::ostream& stream) : CaptureStream(stream, true) {}

CaptureStream::CaptureStream(std::ostream& stream, bool startCapturing) : stream(stream), isCapturing(false) {
  if (startCapturing) {
    start();
  }
}

CaptureStream::~CaptureStream() {
  stop();
}

void CaptureStream::start() {
  if (isActive()) {
    return;
  }
  isCapturing = true;

  // Save the original buffer and redirect the stream to our buffer
  originalBuffer = stream.rdbuf();
  stream.rdbuf(buffer.rdbuf());
}

void CaptureStream::stop() {
  if (!isActive()) {
    return;
  }
  isCapturing = false;

  // Revert the redirection
  stream.rdbuf(originalBuffer);
}

std::string CaptureStream::getCaptured() const {
  stream.flush();
  return buffer.str();
}

bool CaptureStream::isActive() const {
  return isCapturing;
}

#pragma once
#include "macros.hpp"
#include <atomic>
#include <vector>
#include <optional>

class Tick;

class RingBuffer {
  TEST_FRIEND(RingBufferTest);

public:
  RingBuffer();

  auto Insert(const Tick &tick) noexcept -> bool;
  auto Read() noexcept -> std::optional<Tick>;

private:
  std::vector<Tick> buffer_;
  static constexpr int32_t kBUFFER_SIZE_ = 64000;

  std::atomic<int32_t> reader_, writer_;

  auto IsEmpty_() const noexcept -> bool;
  auto IsFull_() const noexcept -> bool;
  auto GetReaderAndWriter_() const noexcept -> std::pair<int32_t, int32_t>;
};

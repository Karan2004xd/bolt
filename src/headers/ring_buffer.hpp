#pragma once
#include "../../include/bolt/macros.hpp"
#include <atomic>
#include <vector>
#include <optional>

namespace bolt {

class Tick;

class RingBuffer {
  TEST_FRIEND(RingBufferTest);

public:
  RingBuffer();

  auto Insert(const Tick &tick) noexcept -> bool;
  auto Read() noexcept -> std::optional<Tick>;

  auto IsEmpty() const noexcept -> bool;
  auto IsFull() const noexcept -> bool;

private:
  std::vector<Tick> buffer_;
  size_t ring_buffer_size_;

  std::atomic<uint64_t> reader_, writer_;
};

}

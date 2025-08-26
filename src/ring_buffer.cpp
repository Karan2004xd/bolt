#include "headers/ring_buffer.hpp"
#include "headers/constants.hpp"

#include "../include/bolt/tick.hpp"

using namespace Constants;

namespace bolt {

RingBuffer::RingBuffer() : reader_(0), writer_(0) {
  ring_buffer_size_ = ::kRING_BUFFER_SIZE;
  buffer_.resize(ring_buffer_size_);
}

auto RingBuffer::Insert(const Tick &tick) noexcept -> bool {
  uint64_t writer_pos;

  do {
    writer_pos = writer_.load(std::memory_order_acquire);
    auto reader_pos = reader_.load(std::memory_order_acquire);

    if (writer_pos - reader_pos == ring_buffer_size_) {
      return false;
    }
  } while (!writer_.compare_exchange_weak(writer_pos,
                                          writer_pos + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));

  auto index = writer_pos % ring_buffer_size_;
  buffer_[index] = tick;
  return true;
}

auto RingBuffer::Read() noexcept -> std::optional<Tick> {
  auto writer_pos = writer_.load(std::memory_order_acquire);
  auto reader_pos = reader_.load(std::memory_order_acquire);

  if (writer_pos == reader_pos) {
    return std::nullopt;
  }

  auto index = reader_pos % ring_buffer_size_;

  auto tick = buffer_[index];
  reader_.fetch_add(1, std::memory_order_acq_rel);
  return tick;
}

auto RingBuffer::IsEmpty() const noexcept -> bool {
  auto writer_pos = writer_.load(std::memory_order_acquire);
  auto reader_pos = reader_.load(std::memory_order_acquire);
  return writer_pos == reader_pos;
}

auto RingBuffer::IsFull() const noexcept -> bool {
  auto writer_pos = writer_.load(std::memory_order_acquire);
  auto reader_pos = reader_.load(std::memory_order_acquire);
  return writer_pos - reader_pos >= ring_buffer_size_;
}

}

#include "../include/ring_buffer.hpp"
#include "../include/tick.hpp"

RingBuffer::RingBuffer() : reader_(0), writer_(0) {
  buffer_.resize(kBUFFER_SIZE_);
}

auto RingBuffer::Insert(const Tick &tick) noexcept -> bool {
  if (IsFull_()) return false;

  const auto &[_, writer_point] = GetReaderAndWriter_();
  buffer_[writer_point] = tick;

  auto next_point = (writer_point + 1) % kBUFFER_SIZE_;
  writer_.store(next_point, std::memory_order_release);

  return true;
}

auto RingBuffer::Read() noexcept -> std::optional<Tick> {
  if (IsEmpty_()) return std::nullopt;
  const auto &[reader_point, _] = GetReaderAndWriter_();
  auto tick = buffer_[reader_point];

  auto next_point = (reader_point + 1) % kBUFFER_SIZE_;
  reader_.store(next_point, std::memory_order_release);
  return tick;
}

auto RingBuffer::IsEmpty_() const noexcept -> bool {
  const auto &[reader_point, writer_point] = GetReaderAndWriter_();
  return reader_point == writer_point;
}

auto RingBuffer::IsFull_() const noexcept -> bool {
  const auto &[reader_point, writer_point] = GetReaderAndWriter_();
  return ((writer_point + 1) % kBUFFER_SIZE_) == reader_point;
}

auto RingBuffer::GetReaderAndWriter_() const noexcept
  -> std::pair<int32_t, int32_t> {
  auto reader_point = reader_.load(std::memory_order_acquire);
  auto writer_point = writer_.load(std::memory_order_acquire);
  return {reader_point, writer_point};
}

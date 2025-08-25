#include "../include/buffer.hpp"
#include "../include/tick.hpp"
#include <numeric>
#include <algorithm>

Buffer::Buffer(size_t reserve_capacity) {
  timestamps_.reserve(reserve_capacity);
  prices_.reserve(reserve_capacity);
  volumes_.reserve(reserve_capacity);

  symbol_ids_.reserve(reserve_capacity);
  exchange_ids_.reserve(reserve_capacity);
  trace_conditions_.reserve(reserve_capacity);
}

Buffer::Buffer(const std::vector<Tick> &ticks) {
  StoreData_(ticks);
}

Buffer::Buffer(const Buffer &other) {
  CopyFrom_(other);
}

Buffer::Buffer(Buffer &&other) noexcept {
  MoveFrom_(std::move(other));
}

auto Buffer::operator=(const Buffer &other) -> Buffer & {
  CopyFrom_(other);
  return *this;
}

auto Buffer::operator=(Buffer &&other) noexcept -> Buffer & {
  MoveFrom_(std::move(other));
  return *this;
}

auto Buffer::operator==(const Buffer &other) const noexcept -> bool {
  return EqualityCheck_(other);
}

auto Buffer::operator!=(const Buffer &other) const noexcept -> bool {
  return !EqualityCheck_(other);
}

auto Buffer::GetTimestamps() const noexcept -> list_cref<uint64_t> {
  return timestamps_;
}

auto Buffer::GetSymbolIds() const noexcept -> list_cref<uint32_t> {
  return symbol_ids_;
}

auto Buffer::GetExchangeIds() const noexcept -> list_cref<uint32_t> {
  return exchange_ids_;
}

auto Buffer::GetPrices() const noexcept -> list_cref<double> {
  return prices_;
}

auto Buffer::GetVolumes() const noexcept -> list_cref<uint32_t> {
  return volumes_;
}

auto Buffer::GetTraceCondtions() const noexcept 
  -> list_cref<TraceConditions> {
  return trace_conditions_;
}

auto Buffer::InsertTick(const Tick &tick) noexcept -> void {
  StoreData_({tick});
}

auto Buffer::Size() const noexcept -> size_t {
  return size_;
}

auto Buffer::Sort(bool ascending) noexcept -> void {
  auto indices = std::vector<size_t>(timestamps_.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto comp = [this, &ascending](const size_t a, const size_t b) {
    if (ascending) return timestamps_[a] < timestamps_[b];
    return timestamps_[a] > timestamps_[b];
  };

  std::sort(indices.begin(), indices.end(), comp);
  auto temp_buffer = Buffer(timestamps_.size());

  for (const auto &index : indices) {
    temp_buffer.timestamps_.push_back(timestamps_[index]);
    temp_buffer.prices_.push_back(prices_[index]);
    temp_buffer.volumes_.push_back(volumes_[index]);

    temp_buffer.symbol_ids_.push_back(symbol_ids_[index]);
    temp_buffer.exchange_ids_.push_back(exchange_ids_[index]);
    temp_buffer.trace_conditions_.push_back(trace_conditions_[index]);
  }

  timestamps_ = std::move(temp_buffer.timestamps_);
  prices_ = std::move(temp_buffer.prices_);
  volumes_ = std::move(temp_buffer.volumes_);

  symbol_ids_ = std::move(temp_buffer.symbol_ids_);
  exchange_ids_ = std::move(temp_buffer.exchange_ids_);
  trace_conditions_ = std::move(temp_buffer.trace_conditions_);
}

auto Buffer::IsSorted() const noexcept -> bool {
  return is_sorted_;
}

auto Buffer::Copy() const noexcept -> Buffer {
  return {*this};
}

auto Buffer::EqualityCheck_(const Buffer &other) const noexcept -> bool {
  if (timestamps_ != other.timestamps_) return false;
  if (prices_ != other.prices_) return false;
  if (volumes_ != other.volumes_) return false;

  if (symbol_ids_ != other.symbol_ids_) return false;
  if (exchange_ids_ != other.exchange_ids_) return false;
  if (trace_conditions_ != other.trace_conditions_) return false;

  return true;
}

auto Buffer::CopyFrom_(const Buffer &other) -> void {
  timestamps_ = other.timestamps_;
  prices_ = other.prices_;
  volumes_ = other.volumes_;

  symbol_ids_ = other.symbol_ids_;
  exchange_ids_ = other.exchange_ids_;
  trace_conditions_ = other.trace_conditions_;

  size_ = other.size_;
  is_sorted_ = other.is_sorted_;
}

auto Buffer::MoveFrom_(Buffer &&other) noexcept -> void {
  timestamps_ = std::move(other.timestamps_);
  prices_ = std::move(other.prices_);
  volumes_ = std::move(other.volumes_);

  symbol_ids_ = std::move(other.symbol_ids_);
  exchange_ids_ = std::move(other.exchange_ids_);
  trace_conditions_ = std::move(other.trace_conditions_);
  is_sorted_ = other.is_sorted_;
  size_ = other.size_;

  other.size_ = {};
  other.is_sorted_ = true;
}

auto Buffer::StoreData_(const std::vector<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {
    if (!timestamps_.empty() && is_sorted_) {
      if (timestamps_.back() > tick.GetTimestamp()) is_sorted_ = false;
    }

    timestamps_.push_back(tick.GetTimestamp());
    prices_.push_back(tick.GetPrice());
    volumes_.push_back(tick.GetVolume());

    symbol_ids_.push_back(tick.GetSymbolId());
    exchange_ids_.push_back(tick.GetExchangeId());
    trace_conditions_.push_back(tick.GetTraceCondtion());
    size_++;
  }
}

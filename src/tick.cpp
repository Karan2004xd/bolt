#include "../include/tick.hpp"
#include <cmath>
#include <cstring>
#include <limits>

Tick::Tick(uint64_t timestamp, double price, uint32_t volume)
  : timestamp_(timestamp), price_(price), volume_(volume) {}

Tick::Tick(uint64_t timestamp, double price, uint32_t volume,
           uint32_t symbol_id, uint32_t exchange_id, TraceConditions trace_condition) {
  timestamp_ = timestamp;
  price_ = price;
  volume_ = volume;

  symbol_id_ = symbol_id;
  exchange_id_ = exchange_id;
  trace_condition_ = trace_condition;
}

auto Tick::operator==(const Tick &other) const noexcept -> bool {
  return check_equality_(other);
}

auto Tick::operator!=(const Tick &other) const noexcept -> bool {
  return !check_equality_(other);
}

auto Tick::GetTimestamp() const noexcept -> uint64_t {
  return timestamp_;
}

auto Tick::GetPrice() const noexcept -> double {
  return price_;
}

auto Tick::GetVolume() const noexcept -> uint32_t {
  return volume_;
}

auto Tick::GetSymbolId() const noexcept -> uint32_t {
  return symbol_id_;
}

auto Tick::GetExchangeId() const noexcept -> uint32_t {
  return exchange_id_;
}

auto Tick::GetTraceCondtion() const noexcept -> const TraceConditions & {
  return trace_condition_;
}

auto Tick::SetTimeStamp(uint64_t timestamp) noexcept -> void {
  timestamp_ = timestamp;
}

auto Tick::SetPrice(double price) noexcept -> void {
  price_ = price;
}

auto Tick::SetVolume(uint32_t volume) noexcept -> void {
  volume_ = volume;
}

auto Tick::SetSymbolId(uint32_t symbol_id) noexcept -> void {
  symbol_id_ = symbol_id;
}

auto Tick::SetExchangeId(uint32_t exchange_id) noexcept -> void {
  exchange_id_ = exchange_id;
}

auto Tick::SetTraceCondtition(const TraceConditions &trace_condition) noexcept -> void {
  trace_condition_ = trace_condition;
}

auto Tick::Serialize(char *buffer) const -> void {
  std::memcpy(buffer, this, GetSerializedSize());
}

auto Tick::Deserialize(const char *buffer) -> void {
  std::memcpy(this, buffer, GetSerializedSize());
}

auto Tick::check_equality_(const Tick &other) const noexcept -> bool {
  constexpr double epsilon = std::numeric_limits<double>::epsilon();

  if (other.timestamp_ != timestamp_) return false;
  if (other.symbol_id_ != symbol_id_) return false;
  if (other.exchange_id_ != exchange_id_) return false;
  if (other.volume_ != volume_) return false;
  if (other.trace_condition_ != trace_condition_) return false;

  if (std::fabs(price_ - other.price_) > epsilon) return false;
  return true;
}

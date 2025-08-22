#include "../include/tick.hpp"
#include <utility>

Tick::Tick(uint64_t timestamp, double price, uint32_t volume)
  : timestamp_(timestamp), price_(price), volume_(volume) {}

Tick::Tick(const Tick &other) {
  copy_from_(other);
}

Tick::Tick(Tick &&other) noexcept {
  copy_from_(other);
  reset_(std::move(other));
}

auto Tick::operator=(const Tick &other) -> Tick {
  copy_from_(other);
  return *this;
}

auto Tick::operator=(Tick &&other) -> Tick {
  copy_from_(other);
  reset_(std::move(other));
  return *this;
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
  auto *current_ptr = buffer;

  current_ptr += AssignValueToPtr_(current_ptr,
                                   timestamp_, sizeof(timestamp_));

  current_ptr += AssignValueToPtr_(current_ptr,
                                   symbol_id_, sizeof(symbol_id_));

  current_ptr += AssignValueToPtr_(current_ptr,
                                   exchange_id_, sizeof(exchange_id_));

  current_ptr += AssignValueToPtr_(current_ptr,
                                   price_, sizeof(price_));

  current_ptr += AssignValueToPtr_(current_ptr,
                                   volume_, sizeof(volume_));

  current_ptr += AssignValueToPtr_(current_ptr,
                                   trace_condition_, sizeof(trace_condition_));
}

auto Tick::Deserialize(const char *buffer) -> void {
  const char *current_ptr = buffer;

  current_ptr += AssignValueFromPtr_(timestamp_,
                                     current_ptr, sizeof(timestamp_));

  current_ptr += AssignValueFromPtr_(symbol_id_,
                                     current_ptr, sizeof(symbol_id_));

  current_ptr += AssignValueFromPtr_(exchange_id_,
                                     current_ptr, sizeof(exchange_id_));

  current_ptr += AssignValueFromPtr_(price_,
                                     current_ptr, sizeof(price_));

  current_ptr += AssignValueFromPtr_(volume_,
                                     current_ptr, sizeof(volume_));

  current_ptr += AssignValueFromPtr_(trace_condition_,
                                     current_ptr, sizeof(trace_condition_));
}

auto Tick::copy_from_(const Tick &other) -> void {
  timestamp_ = other.timestamp_;
  symbol_id_ = other.symbol_id_;
  exchange_id_ = other.exchange_id_;
  price_ = other.price_;
  volume_ = other.volume_;
  trace_condition_ = other.trace_condition_;
}

auto Tick::reset_(Tick &&other) noexcept -> void {
  other.timestamp_ = {};
  other.symbol_id_ = {};
  other.exchange_id_ = {};
  other.price_ = {};
  other.volume_ = {};
  other.trace_condition_ = TraceConditions::kNone;
}

auto Tick::check_equality_(const Tick &other) const noexcept -> bool {
  if (other.timestamp_ != timestamp_) return false;
  if (other.symbol_id_ != symbol_id_) return false;
  if (other.exchange_id_ != exchange_id_) return false;
  if (other.price_ != price_) return false;
  if (other.volume_ != volume_) return false;
  if (other.trace_condition_ != trace_condition_) return false;
  return true;
}

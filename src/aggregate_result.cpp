#include "../include/bolt/aggregate_result.hpp"

namespace bolt {

AggregateResult::AggregateResult(size_t count, uint64_t total_volume,
                                 double max_price, double min_price,
                                 double avg_price, double vwap)
  : count_(count), total_volume_(total_volume), max_price_(max_price),
    min_price_(min_price), avg_price_(avg_price), vwap_(vwap) {}

auto AggregateResult::operator==(
  const AggregateResult &other) const noexcept -> bool {
  return EqualityCheck_(other);
}

auto AggregateResult::operator!=(
  const AggregateResult &other) const noexcept -> bool {
  return !EqualityCheck_(other);
}

auto AggregateResult::SetCount(size_t count) noexcept -> void {
  count_ = count;
}

auto AggregateResult::SetTotalVolume(uint64_t total_volume) noexcept -> void {
  total_volume_ = total_volume;
}

auto AggregateResult::SetAvgPrice(double avg_price) noexcept -> void {
  avg_price_ = avg_price;
}

auto AggregateResult::SetVwap(double vwap) noexcept -> void {
  vwap_ = vwap;
}

auto AggregateResult::SetMinPrice(double min_price) noexcept -> void {
  min_price_ = min_price;
}

auto AggregateResult::SetMaxPrice(double max_price) noexcept -> void {
  max_price_ = max_price;
}

auto AggregateResult::GetCount() const noexcept -> size_t {
  return count_;
}

auto AggregateResult::GetTotalVolume() const noexcept -> uint64_t {
  return total_volume_;
}

auto AggregateResult::GetAvgPrice() const noexcept -> double {
  return avg_price_;
}

auto AggregateResult::GetVwap() const noexcept -> double {
  return vwap_;
}

auto AggregateResult::GetMinPrice() const noexcept -> double {
  return min_price_;
}

auto AggregateResult::GetMaxPrice() const noexcept -> double {
  return max_price_;
}

auto AggregateResult::EqualityCheck_(
  const AggregateResult &other) const noexcept -> bool {

  if (other.count_ != count_) return false;
  if (other.total_volume_ != total_volume_) return false;
  if (other.avg_price_ != avg_price_) return false;
  if (other.max_price_ != max_price_) return false;
  if (other.min_price_ != min_price_) return false;
  if (other.vwap_ != vwap_) return false;

  return true;
}

}

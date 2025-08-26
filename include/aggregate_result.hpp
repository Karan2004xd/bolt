#pragma once

#include <cstddef>
#include <cstdint>
#include "macros.hpp"

class AggregateResult {
  TEST_FRIEND(AggregateResultTest);

public:
  AggregateResult() = default;
  AggregateResult(size_t count, uint64_t total_volume,
                  double max_price, double min_price,
                  double avg_price, double vwap);

  AggregateResult(const AggregateResult &) = default;
  AggregateResult(AggregateResult &&) = default;

  auto operator=(const AggregateResult &) -> AggregateResult & = default;
  auto operator=(AggregateResult &&) noexcept -> AggregateResult & = default;

  auto operator==(const AggregateResult &other) const noexcept -> bool;
  auto operator!=(const AggregateResult &other) const noexcept -> bool;

  auto SetCount(size_t count) noexcept -> void;
  auto SetTotalVolume(uint64_t total_volume) noexcept -> void;
  auto SetAvgPrice(double avg_price) noexcept -> void;
  auto SetVwap(double vwap) noexcept -> void;
  auto SetMinPrice(double min_price) noexcept -> void;
  auto SetMaxPrice(double max_price) noexcept -> void;

  auto GetCount() const noexcept -> size_t;
  auto GetTotalVolume() const noexcept -> uint64_t;
  auto GetAvgPrice() const noexcept -> double;
  auto GetVwap() const noexcept -> double;
  auto GetMinPrice() const noexcept -> double;
  auto GetMaxPrice() const noexcept -> double;

private:
  size_t count_ {};
  uint64_t total_volume_ {};
  double max_price_ {};
  double min_price_ {};
  double avg_price_ {};
  double vwap_ {};

  auto EqualityCheck_(const AggregateResult &other) const noexcept -> bool;
};

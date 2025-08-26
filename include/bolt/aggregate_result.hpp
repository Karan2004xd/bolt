#pragma once

#include <cstddef>
#include <cstdint>
#include "macros.hpp"

/**
* @file aggregate_result.hpp
* @brief Defines Result class which holds the relevant data for aggregate query results,
*        returned by the 'Database' class aggregate methods.
*/

namespace bolt {

/**
  * @class AggregateResult
  * @brief Handles the data obtained from the aggregate quries.
  *
  * This class provides clean and safe way to access the underlying aggregated data.
  */
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

  /**
  * @brief Sets the total entries aggregate result this object holds.
  *
  * @param count The count of total entries.
  */
  auto SetCount(size_t count) noexcept -> void;

  /**
  * @brief Sets the total volume, which is the sum of all 
  *        the volumes in the given range (for which the object is defined for).
  *
  * @param total_volume The sum of all the volumes of the defined range.
  */
  auto SetTotalVolume(uint64_t total_volume) noexcept -> void;

  /**
  * @brief Sets the average value of the prices stored.
  *
  * @param avg_price The average of prices to be stored.
  */
  auto SetAvgPrice(double avg_price) noexcept -> void;

  /**
  * @brief Sets the Volume Weighted Average Price (VWAP)
  *
  * @param avg_price The vwap value to store.
  */
  auto SetVwap(double vwap) noexcept -> void;

  /**
  * @brief Sets the minimum price value.
  *
  * @param avg_price The minimum price value to store.
  */
  auto SetMinPrice(double min_price) noexcept -> void;

  /**
  * @brief Sets the maximum price value.
  *
  * @param avg_price The maximum price value to store.
  */
  auto SetMaxPrice(double max_price) noexcept -> void;

  /**
  * @brief Gets the length of the range on which the object is defined or set on.
  *
  * @return The length integer indicating the size.
  */
  auto GetCount() const noexcept -> size_t;

  /**
  * @brief Gets the total volume which is the sum of all volumes in the range.
  *
  * @return The total volume value.
  */
  auto GetTotalVolume() const noexcept -> uint64_t;

  /**
  * @brief Gets the average of the prices stored.
  *
  * @return The average value of the prices.
  */
  auto GetAvgPrice() const noexcept -> double;

  /**
  * @brief Gets Volume Weighted Average Price of the stored range.
  *
  * @return The vwap value.
  */
  auto GetVwap() const noexcept -> double;

  /**
  * @brief Get the minimum stored price value.
  *
  * @return The minimum price value.
  */
  auto GetMinPrice() const noexcept -> double;

  /**
  * @brief Get the maximum stored price value.
  *
  * @return The maximum price value.
  */
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

}

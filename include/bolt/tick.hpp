#pragma once
#include "macros.hpp"
#include "trade_conditions.hpp"
#include <type_traits>
#include <cstddef>

/**
* @file tick.hpp
* @brief Defines the core data schema and container that will be stored inside the database.
*/
namespace bolt {

/**
  * @class Tick
  * @brief Handles the raw data provided by the user to be stored in proper format
  *        and be accessed and edited in a safe and clean manner.
  */
class Tick {
  TEST_FRIEND(TickTest);

public:
  Tick() = default;
  Tick(uint64_t timestamp, double price, uint32_t volume);
  Tick(uint64_t timestamp, double price, uint32_t volume,
       uint32_t symbol_id, uint32_t exchange_id, TradeConditions trace_condition);

  Tick(const Tick &other) = default;
  Tick(Tick &&other) noexcept = default;

  auto operator=(const Tick &other) -> Tick & = default;
  auto operator=(Tick &&other) -> Tick & = default;

  auto operator==(const Tick &other) const noexcept -> bool;
  auto operator!=(const Tick &other) const noexcept -> bool;

  /**
  * @brief Returns the stored timestamp (when the data arrived).
  *
  * @return A numeric timestamp value.
  */
  auto GetTimestamp() const noexcept -> uint64_t;

  /**
  * @brief Returns the stored price value (the stock price).
  *
  * @return A numeric price value.
  */
  auto GetPrice() const noexcept -> double;

  /**
  * @brief Returns the stored volume value (the amount of stocks).
  *
  * @return A numeric volume value.
  */
  auto GetVolume() const noexcept -> uint32_t;

  /**
  * @brief Returns the symbol id (the company stock symbol but numeric e.g. GOOG).
  *
  * @return A numeric value of symbol id.
  */
  auto GetSymbolId() const noexcept -> uint32_t;

  /**
  * @brief Returns the exchange id (the company stock symbol but numeric e.g. GOOG).
  *
  * @return A numeric value of exchange id.
  */
  auto GetExchangeId() const noexcept -> uint32_t;

  /**
  * @brief Returns the trade condtions (the stock market metadata about a trade).
  *
  * @return A numeric trace condtion code.
  */
  auto GetTradeCondition() const noexcept -> const TradeConditions &;


  /**
  * @brief Sets the timestamp of the current Tick object.
  *
  * @param timestamp The timestamp to store.
  */
  auto SetTimeStamp(uint64_t timestamp) noexcept -> void;

  /**
  * @brief Sets the price of the current Tick object.
  *
  * @param price The price value to store.
  */
  auto SetPrice(double price) noexcept -> void;

  /**
  * @brief Sets the volume of the current Tick object.
  *
  * @param volume The volume value to store.
  */
  auto SetVolume(uint32_t volume) noexcept -> void;

  /**
  * @brief Sets the symbol id of the current Tick object.
  *
  * @param symbol_id The symbol id value to store.
  */
  auto SetSymbolId(uint32_t symbol_id) noexcept -> void;

  /**
  * @brief Sets the exchange id of the current Tick object.
  *
  * @param exchange_id The exchange id value to store.
  */
  auto SetExchangeId(uint32_t exchange_id) noexcept -> void;

  /**
  * @brief Sets the trade condition of the current Tick object.
  *
  * @param trade_condition The trade condition value to store.
  */
  auto SetTradeCondition(const TradeConditions &trade_condition) noexcept -> void;

  /**
  * @brief Serializes the current object into the char * format.
  *
  * @param buffer A mutable char pointer (char *) in which the serialization will be stored.
  * @note The current object will not be affected by this call.
  */
  auto Serialize(char *buffer) const -> void;

  /**
  * @brief Deserializes provided serialized Tick buffer to object.
  *
  * @param buffer A immutable char pointer (const char *) which holds a Tick object serialization.
  * @note The provided serialized object will be reflected onto the current object.
  */
  auto Deserialize(const char *buffer) -> void;

  /**
  * @brief Returns the total minimum size occupied by the Tick object.
  *
  * @return The minimum size occupied by the Tick object.
  */
  static constexpr auto GetSerializedSize() noexcept -> size_t {
    return sizeof(uint64_t)
    + (sizeof(uint32_t) * 3)
    + sizeof(double)
    + sizeof(TradeConditions);
  }

private:
  uint64_t timestamp_ {};
  uint32_t symbol_id_ {};
  uint32_t exchange_id_ {};
  double price_ {};
  uint32_t volume_ {};
  TradeConditions trade_condition_ {TradeConditions::kNone};

  auto check_equality_(const Tick &tick) const noexcept -> bool;
};

static_assert(std::is_standard_layout_v<Tick> && std::is_trivially_copyable_v<Tick>,
              "Tick must be a standard-layout and trivially copyable type for safe serialization");

}

#pragma once
#include "macros.hpp"
#include "trace_conditions.hpp"
#include <type_traits>
#include <cstddef>

class Tick {
  TEST_FRIEND(TickTest);

public:
  Tick() = default;
  Tick(uint64_t timestamp, double price, uint32_t volume);
  Tick(uint64_t timestamp, double price, uint32_t volume,
       uint32_t symbol_id, uint32_t exchange_id, TraceConditions trace_condition);

  Tick(const Tick &other) = default;
  Tick(Tick &&other) noexcept = default;

  auto operator=(const Tick &other) -> Tick & = default;
  auto operator=(Tick &&other) -> Tick & = default;

  auto operator==(const Tick &other) const noexcept -> bool;
  auto operator!=(const Tick &other) const noexcept -> bool;

  auto GetTimestamp() const noexcept -> uint64_t;
  auto GetPrice() const noexcept -> double;
  auto GetVolume() const noexcept -> uint32_t;
  auto GetSymbolId() const noexcept -> uint32_t;
  auto GetExchangeId() const noexcept -> uint32_t;
  auto GetTraceCondtion() const noexcept -> const TraceConditions &;

  auto SetTimeStamp(uint64_t timestamp) noexcept -> void;
  auto SetPrice(double price) noexcept -> void;
  auto SetVolume(uint32_t volume) noexcept -> void;
  auto SetSymbolId(uint32_t symbol_id) noexcept -> void;
  auto SetExchangeId(uint32_t exchange_id) noexcept -> void;
  auto SetTraceCondtition(const TraceConditions &trace_condition) noexcept -> void;

  auto Serialize(char *buffer) const -> void;
  auto Deserialize(const char *buffer) -> void;

  static constexpr auto GetSerializedSize() noexcept -> size_t {
    return sizeof(uint64_t)
    + (sizeof(uint32_t) * 3)
    + sizeof(double)
    + sizeof(TraceConditions);
  }

private:
  uint64_t timestamp_ {};
  uint32_t symbol_id_ {};
  uint32_t exchange_id_ {};
  double price_ {};
  uint32_t volume_ {};
  TraceConditions trace_condition_ {TraceConditions::kNone};

  auto check_equality_(const Tick &tick) const noexcept -> bool;
};

static_assert(std::is_standard_layout_v<Tick> && std::is_trivially_copyable_v<Tick>,
              "Tick must be a standard-layout and trivially copyable type for safe serialization");

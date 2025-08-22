#pragma once
#include "macros.hpp"
#include "trace_conditions.hpp"
#include <cstring>

class Tick {
  TEST_FRIEND(TickTest);

public:
  Tick() = default;
  Tick(uint64_t timestamp, double price, uint32_t volume);

  Tick(const Tick &other);
  Tick(Tick &&other) noexcept;

  auto operator=(const Tick &other) -> Tick;
  auto operator=(Tick &&other) -> Tick;

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

  template <typename T>
  auto AssignValueToPtr_(char *dest_ptr,
                         T &src,
                         size_t src_size) const -> size_t {
    std::memcpy(dest_ptr, &src, src_size);
    return src_size;
  }

  template <typename T>
  auto AssignValueFromPtr_(T &dest,
                           const char *src_ptr,
                           size_t dest_size) const -> size_t {
    std::memcpy(&dest, src_ptr, dest_size);
    return dest_size;
  }

  auto copy_from_(const Tick &tick) -> void;
  auto reset_(Tick &&tick) noexcept -> void;

  auto check_equality_(const Tick &tick) const noexcept -> bool;
};

#pragma once

#include <vector>
#include "../../include/bolt/trade_conditions.hpp"
#include "../../include/bolt/macros.hpp"

namespace bolt {

class Tick;

class Buffer {
  TEST_FRIEND(BufferTest);

public:
  template <typename T>
  using list_cref = const std::vector<T> &;

  Buffer() = default;

  Buffer(size_t reserve_capacity);
  Buffer(const std::vector<Tick> &ticks);

  Buffer(const Buffer &other);
  Buffer(Buffer &&other) noexcept;

  auto operator=(const Buffer &other) -> Buffer &;
  auto operator=(Buffer &&other) noexcept -> Buffer &;

  auto operator==(const Buffer &other) const noexcept -> bool;
  auto operator!=(const Buffer &other) const noexcept -> bool;

  auto GetTimestamps() const noexcept -> list_cref<uint64_t>;
  auto GetSymbolIds() const noexcept -> list_cref<uint32_t>;
  auto GetExchangeIds() const noexcept -> list_cref<uint32_t>;

  auto GetPrices() const noexcept -> list_cref<double>;
  auto GetVolumes() const noexcept -> list_cref<uint32_t>;
  auto GetTraceCondtions() const noexcept -> list_cref<TradeConditions>;

  auto InsertTick(const Tick &tick) noexcept -> void;

  auto Size() const noexcept -> size_t;
  auto Sort(bool ascending = true) noexcept -> void;
  auto Copy() const noexcept -> Buffer;

  auto IsSorted() const noexcept -> bool;

private:
  std::vector<uint64_t> timestamps_;
  std::vector<uint32_t> symbol_ids_;
  std::vector<uint32_t> exchange_ids_;
  std::vector<double> prices_;
  std::vector<uint32_t> volumes_;
  std::vector<TradeConditions> trace_conditions_;

  uint64_t size_ {};
  bool is_sorted_ {true};

  auto StoreData_(const std::vector<Tick> &ticks) noexcept -> void;
  auto EqualityCheck_(const Buffer &other) const noexcept -> bool;

  auto CopyFrom_(const Buffer &other) -> void;
  auto MoveFrom_(Buffer &&other) noexcept -> void;
};

}

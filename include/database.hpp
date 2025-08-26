#pragma once
#include <condition_variable>
#include <functional>
#include "macros.hpp"

class RingBuffer;
class ThreadPool;
class BufferManager;
class Tick;
class State;
class Buffer;
class AggregateResult;

class Database {
  TEST_FRIEND(DatabaseTest);

public:
  using filter_func = std::function<bool(const Tick &)>;

  Database();

  Database(const Database &) = delete;
  Database(Database &&) = delete;

  auto operator=(const Database &) = delete;
  auto operator=(Database &&) = delete;

  auto Insert(const std::vector<Tick> &ticks) noexcept -> void;
  auto Insert(const Tick &tick) noexcept -> void;

  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts) -> std::vector<Tick>;

  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts,
                   const filter_func &filter) -> std::vector<Tick>;

  auto Aggregate(uint64_t start_ts,
                 uint64_t end_ts) -> AggregateResult;

  auto Aggregate(uint64_t start_ts,
                 uint64_t end_ts,
                 const filter_func &filter) -> AggregateResult;

  auto Size() const noexcept -> size_t;
  auto Flush() noexcept -> void;

  ~Database();

private:
  std::atomic<bool> stop_insert_thread_;

  mutable std::mutex insert_thread_mutex_;
  std::condition_variable data_added_to_buffer_;

  std::shared_ptr<RingBuffer> data_buffer_;
  std::shared_ptr<ThreadPool> thread_pool_;
  std::shared_ptr<BufferManager> storage_handler_;

  auto StartInsertThread_() noexcept -> void;
  auto InsertBase_(const std::vector<Tick> &ticks) noexcept -> void;

  auto GetTicksFromActiveBuffer_(
    const std::shared_ptr<const State> &state,
    uint64_t start_ts,
    uint64_t end_ts,
    const filter_func &filter = [](const Tick &){ return true;})
    -> std::pair<bool, std::vector<Tick>>;

  auto GetTicksFromSealedBuffer_(
    const std::shared_ptr<const State> &state,
    uint64_t start_ts,
    uint64_t end_ts,
    const filter_func &filter = [](const Tick &){ return true;})
    -> std::pair<bool, std::vector<Tick>>;

  auto CheckAndSetTick_(std::vector<Tick> &ticks,
                        size_t index,
                        const std::shared_ptr<Buffer> &buffer,
                        const filter_func &filter) -> void;

  auto GetSortedTicks_(
    uint64_t start_ts, uint64_t end_ts,
    const std::shared_ptr<const State> &state,
    const filter_func &filter = [](const Tick &){ return true;}) -> std::vector<Tick>;

  auto SetAggregateObj_(AggregateResult &result,
                        const std::vector<Tick> &sorted_ticks) const noexcept -> void;
};

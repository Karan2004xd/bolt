#pragma once
#include <condition_variable>
#include <functional>
#include <optional>
#include "macros.hpp"

class RingBuffer;
class ThreadPool;
class BufferManager;
class Tick;

class Database {
  TEST_FRIEND(DatabaseTest);

public:
  Database();

  Database(const Database &) = delete;
  Database(Database &&) = delete;

  auto operator=(const Database &) = delete;
  auto operator=(Database &&) = delete;

  auto Insert(const std::vector<Tick> &ticks) noexcept -> void;
  auto Insert(const Tick &tick) noexcept -> void;

  auto GetAt(uint64_t ts) const noexcept -> std::optional<Tick>;

  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts) const noexcept 
    -> std::vector<Tick>;

  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts,
                   std::function<bool(const Tick &)> &&filter) const noexcept 
    -> std::vector<Tick>;

  auto Size() const noexcept -> size_t;
  auto Flush() noexcept -> void;

  ~Database();

private:
  bool stop_insert_thread_ {false};
  mutable std::mutex insert_thread_mutex_;
  std::condition_variable data_added_to_buffer_;

  static std::unique_ptr<RingBuffer> data_buffer_;
  static std::unique_ptr<ThreadPool> thread_pool_;
  static std::shared_ptr<BufferManager> storage_handler_;

  auto StartInsertThread_() noexcept -> void;
  auto InsertBase_(const std::vector<Tick> &ticks) noexcept -> void;
};

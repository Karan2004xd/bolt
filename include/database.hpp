#pragma once
#include <condition_variable>
#include <vector>
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

  ~Database();

private:
  bool stop_insert_thread_ {false};
  mutable std::mutex insert_thread_mutex_;
  std::condition_variable data_added_to_buffer_;

  static std::shared_ptr<RingBuffer> data_buffer_;
  static std::unique_ptr<ThreadPool> thread_pool_;
  static std::unique_ptr<BufferManager> storage_handler_;

  auto StartInsertThread_() noexcept -> void;
  auto InsertBase_(const std::vector<Tick> &ticks) noexcept -> void;
};

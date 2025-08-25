#pragma once

#include "macros.hpp"
#include <mutex>
#include <deque>
#include <memory>
#include <vector>

class Buffer;
class Tick;
class ThreadPool;
class State;

class BufferManager {
  TEST_FRIEND(BufferManagerTest);
  TEST_FRIEND(DatabaseTest);

public:
  template <typename T>
  using ptr = std::shared_ptr<T>;

  using const_buffer = ptr<Buffer>;
  using sealed_list = std::deque<const_buffer>;

  BufferManager(ThreadPool &pool);

  auto Insert(const std::vector<Tick> &ticks) noexcept -> void;
  auto Insert(const Tick &tick) noexcept -> void;

  auto GetState() const noexcept -> std::shared_ptr<const State>;

private:
  int32_t maximum_sealed_buffers_;
  int32_t maximum_buffer_size_;
  mutable std::mutex background_mutex_;

  ThreadPool &pool_;

  ptr<sealed_list> sealed_buffers_;

  ptr<Buffer> active_buffer_;
  std::atomic<ptr<const State>> current_state_;

  auto InsertBase_(const Tick &tick) noexcept -> void;
  auto SetNewState_(ptr<Buffer> &&new_sealed_buffer) noexcept -> void;
};

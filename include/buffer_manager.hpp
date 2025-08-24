#pragma once

#include "macros.hpp"
#include <atomic>
#include <deque>
#include <memory>
#include <vector>

class Buffer;
class Tick;
class ThreadPool;
class State;

class BufferManager {
  TEST_FRIEND(BufferManagerTest);

public:
  template <typename T>
  using ptr = std::shared_ptr<T>;

  using const_buffer = ptr<Buffer>;
  using sealed_list = std::deque<const_buffer>;

  template <typename T>
  using list = std::initializer_list<T>;

  BufferManager(ThreadPool &pool);
  BufferManager(const list<Tick> &ticks, ThreadPool &pool);

  auto Insert(const list<Tick> &ticks) noexcept -> void;
  auto Insert(const std::vector<Tick> &ticks) noexcept -> void;

  auto GetState() noexcept -> std::shared_ptr<State>;

private:
  int32_t maximum_sealed_buffers_;
  int32_t maximum_buffer_size_;

  ThreadPool &pool_;

  std::atomic<ptr<sealed_list>> sealed_buffers_ {
    std::make_shared<sealed_list>()
  };
  ptr<Buffer> active_buffer_ = std::make_shared<Buffer>();

  auto InsertBase_(const std::vector<Tick> &ticks) noexcept -> void;
};

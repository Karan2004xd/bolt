#pragma once

#include "macros.hpp"
#include <atomic>
#include <deque>
#include <memory>

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
  auto GetState() noexcept -> std::shared_ptr<State>;

private:
  static constexpr int16_t kMAXIMUM_SEALED_BUFFERS_ = 100;
  static constexpr int16_t kMAXIMUM_SEALED_BUFFER_SIZE_ = 10000;

  ThreadPool &pool_;

  std::atomic<ptr<sealed_list>> sealed_buffers_ {
    std::make_shared<sealed_list>()
  };
  ptr<Buffer> active_buffer_ {nullptr};

  auto InsertBase_(const list<Tick> &ticks) noexcept -> void;
};

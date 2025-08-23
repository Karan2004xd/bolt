#include "../include/buffer_manager.hpp"
#include "../include/buffer.hpp"
#include "../include/tick.hpp"
#include "../include/thread_pool.hpp"
#include "../include/state.hpp"

BufferManager::BufferManager(ThreadPool &pool) : pool_(pool) {}

BufferManager::BufferManager(const list<Tick> &ticks,
                             ThreadPool &pool) : pool_(pool) {
  InsertBase_(ticks);
}

auto BufferManager::Insert(const list<Tick> &ticks) noexcept -> void {
  InsertBase_(ticks);
}

auto BufferManager::GetState() noexcept -> std::shared_ptr<State> {
  auto sealed_buffers = sealed_buffers_.load(std::memory_order_acquire);
  auto active_buffer = active_buffer_;

  return std::make_shared<State>(active_buffer, sealed_buffers);
}

auto BufferManager::InsertBase_(const list<Tick> &ticks) noexcept -> void {
  auto curr_sealed_buffers = sealed_buffers_.load(std::memory_order_acquire);

  if (curr_sealed_buffers->size() > kMAXIMUM_SEALED_BUFFERS_) {
    auto new_sealed_buffers = std::make_shared<sealed_list>(*curr_sealed_buffers);
    new_sealed_buffers->pop_front();
    sealed_buffers_.store(new_sealed_buffers, std::memory_order_release);
  }

  if (active_buffer_->Size() > kMAXIMUM_SEALED_BUFFER_SIZE_) {
    auto func = [buffer_to_seal = std::make_shared<Buffer>(active_buffer_->Copy()), this]() mutable {
      buffer_to_seal->Sort();

      auto curr_sealed_buffers = sealed_buffers_.load(std::memory_order_acquire);
      auto new_sealed_buffers = std::make_shared<sealed_list>(*curr_sealed_buffers);
      new_sealed_buffers->push_back(std::move(buffer_to_seal));

      sealed_buffers_.store(new_sealed_buffers, std::memory_order_release);
    };

    pool_.AssignTask(func);

    active_buffer_ = std::make_shared<Buffer>();
  }

  for (const auto &tick : ticks) {
    active_buffer_->InsertTick(tick);
  }
}

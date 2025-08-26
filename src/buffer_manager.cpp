#include "headers/buffer_manager.hpp"
#include "headers/buffer.hpp"
#include "headers/thread_pool.hpp"
#include "headers/state.hpp"
#include "headers/constants.hpp"

#include "../include/bolt/tick.hpp"

using namespace Constants;

namespace bolt {

BufferManager::BufferManager(ThreadPool &pool) : pool_(pool) {
  maximum_sealed_buffers_ = ::kMAXIMUM_SEALED_BUFFERS;
  maximum_buffer_size_ = ::kMAXIMUM_SEALED_BUFFER_SIZE;

  current_state_ = std::make_shared<const State>();
  sealed_buffers_ = std::make_shared<sealed_list>();
  active_buffer_ = std::make_shared<Buffer>();
}

auto BufferManager::Insert(const Tick &tick) noexcept -> void {
  InsertBase_(tick);
}

auto BufferManager::Insert(const std::vector<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {
    InsertBase_(tick);
  }
}

auto BufferManager::GetState() const noexcept -> std::shared_ptr<const State> {
  return current_state_.load(std::memory_order_acquire);
}

auto BufferManager::SetNewState_(ptr<Buffer> &&new_sealed_buffer) noexcept -> void {
  std::shared_ptr<const State> new_state;
  {
    auto lock = std::unique_lock<std::mutex>(background_mutex_);
    if (new_sealed_buffer) {
      sealed_buffers_->emplace_back(std::move(new_sealed_buffer));

      if (sealed_buffers_->size() >= size_t(maximum_sealed_buffers_)) {
        sealed_buffers_->pop_front();
      }
    }
    new_state = std::make_shared<const State>(active_buffer_, sealed_buffers_);
  }
  current_state_.store(std::move(new_state), std::memory_order_acquire);
}

auto BufferManager::InsertBase_(const Tick &tick) noexcept -> void {
  active_buffer_->InsertTick(tick);

  if (active_buffer_->Size() >= size_t(maximum_buffer_size_)) {
    auto buffer_to_seal = std::make_shared<Buffer>(maximum_buffer_size_);
    std::swap(active_buffer_, buffer_to_seal);

    auto sealing_task = [this, sealed_buffer = std::move(buffer_to_seal)]() mutable {
      if (!sealed_buffer->IsSorted()) {
        sealed_buffer->Sort();
      }
      SetNewState_(std::move(sealed_buffer));
    };
    pool_.AssignTask(std::move(sealing_task));

  } else {
    pool_.AssignTask([this]{
      SetNewState_(nullptr);
    });
  }
}

}

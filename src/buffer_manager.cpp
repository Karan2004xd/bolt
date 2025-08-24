#include "../include/buffer_manager.hpp"
#include "../include/buffer.hpp"
#include "../include/tick.hpp"
#include "../include/thread_pool.hpp"
#include "../include/state.hpp"
#include "../include/constants.hpp"

using namespace Constants;

BufferManager::BufferManager(ThreadPool &pool) : pool_(pool) {
  maximum_sealed_buffers_ = ::kMAXIMUM_SEALED_BUFFERS;
  maximum_buffer_size_ = ::kMAXIMUM_SEALED_BUFFER_SIZE;
}

BufferManager::BufferManager(const list<Tick> &ticks,
                             ThreadPool &pool) : pool_(pool) {
  maximum_sealed_buffers_ = ::kMAXIMUM_SEALED_BUFFERS;
  maximum_buffer_size_ = ::kMAXIMUM_SEALED_BUFFER_SIZE;

  for (const auto &tick : ticks) {
    InsertBase_(tick);
  }
}

auto BufferManager::Insert(const list<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {
    InsertBase_(tick);
  }
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
  auto new_state = std::shared_ptr<State>();
  {
    auto lock = std::unique_lock<std::mutex>(background_mutex_);
    sealed_buffers_->emplace_back(std::move(new_sealed_buffer));

    if (sealed_buffers_->size() >= size_t(maximum_sealed_buffers_)) {
      sealed_buffers_->pop_front();
    }
    new_state = std::make_shared<State>(active_buffer_, sealed_buffers_);
  }
  current_state_.store(std::move(new_state), std::memory_order_acquire);
}

auto BufferManager::InsertBase_(const Tick &tick) noexcept -> void {
  active_buffer_->InsertTick(tick);

  if (active_buffer_->Size() >= size_t(maximum_buffer_size_)) {
    auto buffer_to_seal = std::make_shared<Buffer>(maximum_buffer_size_);
    std::swap(active_buffer_, buffer_to_seal);

    auto sealing_task = [this, sealed_buffer = std::move(buffer_to_seal)]() mutable {
      sealed_buffer->Sort();
      SetNewState_(std::move(sealed_buffer));
    };

    pool_.AssignTask(std::move(sealing_task));
  }
}

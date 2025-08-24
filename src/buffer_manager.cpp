#include "../include/buffer_manager.hpp"
#include "../include/buffer.hpp"
#include "../include/tick.hpp"
#include "../include/thread_pool.hpp"
#include "../include/state.hpp"
#include "../include/constants.hpp"

using namespace Constants;

BufferManager::BufferManager(ThreadPool &pool) : pool_(pool) {
  maximum_sealed_buffers_ = ::kMAXIMUM_SEALED_BUFFERS_;
  maximum_buffer_size_ = ::kMAXIMUM_SEALED_BUFFER_SIZE_;
}

BufferManager::BufferManager(const list<Tick> &ticks,
                             ThreadPool &pool) : pool_(pool) {
  maximum_sealed_buffers_ = ::kMAXIMUM_SEALED_BUFFERS_;
  maximum_buffer_size_ = ::kMAXIMUM_SEALED_BUFFER_SIZE_;
  InsertBase_(ticks);
}

auto BufferManager::Insert(const list<Tick> &ticks) noexcept -> void {
  InsertBase_(ticks);
}

auto BufferManager::Insert(const std::vector<Tick> &ticks) noexcept -> void {
  InsertBase_(ticks);
}

auto BufferManager::GetState() noexcept -> std::shared_ptr<State> {
  auto sealed_buffers = sealed_buffers_.load(std::memory_order_acquire);
  auto active_buffer = active_buffer_;

  return std::make_shared<State>(active_buffer, sealed_buffers);
}

auto BufferManager::InsertBase_(const std::vector<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {

    if (active_buffer_->Size() >= size_t(maximum_buffer_size_)) {
      auto func = [&, buffer_to_seal = std::make_shared<Buffer>(active_buffer_->Copy())]() {

        buffer_to_seal->Sort();
        auto curr_sealed_buffers = sealed_buffers_.load(std::memory_order_acquire);

        while (true) {
          auto new_sealed_buffers = std::make_shared<sealed_list>(*curr_sealed_buffers);
          new_sealed_buffers->emplace_back(std::move(buffer_to_seal));

          if (curr_sealed_buffers->size() >= size_t(maximum_sealed_buffers_)) {
            new_sealed_buffers->pop_front();
          }

          if (sealed_buffers_.compare_exchange_weak(curr_sealed_buffers,
                                                    new_sealed_buffers,
                                                    std::memory_order_release,
                                                    std::memory_order_acquire)) {
            break;
          }
        }
      };

      pool_.AssignTask(std::move(func));
      active_buffer_ = std::make_shared<Buffer>();
    }
    active_buffer_->InsertTick(tick);
  }
}

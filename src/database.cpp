#include "../include/database.hpp"
#include "../include/thread_pool.hpp"
#include "../include/ring_buffer.hpp"
#include "../include/buffer_manager.hpp"
#include "../include/tick.hpp"
#include "../include/buffer.hpp"
#include "../include/state.hpp"

std::shared_ptr<RingBuffer> Database::data_buffer_;
std::unique_ptr<ThreadPool> Database::thread_pool_;
std::unique_ptr<BufferManager> Database::storage_handler_;

Database::Database() {
  data_buffer_ = std::make_shared<RingBuffer>();
  thread_pool_ = std::make_unique<ThreadPool>();
  storage_handler_ = std::make_unique<BufferManager>(*thread_pool_);

  StartInsertThread_();
}

Database::~Database() {
  stop_insert_thread_ = true;
  thread_pool_->Wait();
}

auto Database::Insert(const std::vector<Tick> &ticks) noexcept -> void {
  InsertBase_(ticks);
}

auto Database::Insert(const Tick &tick) noexcept -> void {
  InsertBase_({tick});
}

auto Database::StartInsertThread_() noexcept -> void {
  thread_pool_->AssignTask([this]{
    while (!stop_insert_thread_) {
      {
        auto lock = std::unique_lock<std::mutex>(insert_thread_mutex_);
        data_added_to_buffer_.wait(lock, [&]{
          return !data_buffer_->IsEmpty() || stop_insert_thread_; 
        });
      }

      if (!data_buffer_->IsEmpty() && !stop_insert_thread_) {
        auto tick_opt = std::optional<Tick>();
        while (!(tick_opt = data_buffer_->Read())) {
          std::this_thread::yield();
        }
        storage_handler_->Insert({*tick_opt});
      }
    }
  });
}

auto Database::InsertBase_(const std::vector<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {
    data_buffer_->Insert(tick);
  }
  data_added_to_buffer_.notify_one();
}

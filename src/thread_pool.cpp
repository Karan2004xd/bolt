#include "../include/thread_pool.hpp"

ThreadPool::ThreadPool() : stop_workers_(false) {
  workers_.resize(kNUMBER_OF_THREADS);

  for (int i = 0; i < kNUMBER_OF_THREADS; i++) {
    workers_[i] = std::thread(&ThreadPool::StartWorker_, this);
  }
}

auto ThreadPool::Wait() noexcept -> void {
  StopWorkersBase_();
}

ThreadPool::~ThreadPool() {
  StopWorkersBase_();
}

auto ThreadPool::StopWorkersBase_() noexcept -> void {
  stop_workers_ = true;
  condtion_to_allow_.notify_all();

  for (int i = 0; i < kNUMBER_OF_THREADS; i++) {
    if (workers_[i].joinable()) {
      workers_[i].join();
    }
  }
}

auto ThreadPool::StartWorker_() noexcept -> void {
  while (true) {
    auto task = std::function<void()>();

    {
      auto lock = std::unique_lock<std::mutex>(pool_mutex_);
      condtion_to_allow_.wait(lock, [&]{
        return stop_workers_ || !tasks_.empty();
      });

      if (stop_workers_ && tasks_.empty()) return;
      task = std::move(tasks_.front());
      tasks_.pop();
    }
    task();
  }
}

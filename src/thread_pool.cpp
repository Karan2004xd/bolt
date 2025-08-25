#include "../include/thread_pool.hpp"
#include "../include/constants.hpp"

using namespace Constants;

ThreadPool::ThreadPool() : stop_workers_(false) {
  auto system_threads = std::thread::hardware_concurrency() / 2;
  number_of_threads_ =
    system_threads > kMINIMUM_THREADS ? system_threads : kMINIMUM_THREADS;

  StartPoolBase_();
}

auto ThreadPool::Restart() noexcept -> void {
  StopWorkersBase_();
  StartPoolBase_();
}

auto ThreadPool::Shutdown() noexcept -> void {
  StopWorkersBase_();
}

ThreadPool::~ThreadPool() {
  StopWorkersBase_();
}

auto ThreadPool::StopWorkersBase_() noexcept -> void {
  stop_workers_.store(true, std::memory_order_release);
  condition_to_allow_.notify_all();

  for (int i = 0; i < number_of_threads_; i++) {
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
      condition_to_allow_.wait(lock, [&]{
        return stop_workers_.load() || !tasks_.empty();
      });

      if (stop_workers_.load() && tasks_.empty()) return;
      task = std::move(tasks_.front());
      tasks_.pop();
    }
    task();
  }
}

auto ThreadPool::StartPoolBase_() noexcept -> void {
  stop_workers_.store(false, std::memory_order_release);

  workers_.resize(number_of_threads_);
  for (int i = 0; i < number_of_threads_; i++) {
    workers_[i] = std::thread(&ThreadPool::StartWorker_, this);
  }
}

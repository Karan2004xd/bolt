#pragma once

#include "macros.hpp"
#include <future>
#include <queue>
#include <functional>

class ThreadPool {
  TEST_FRIEND(ThreadPoolTest);

public:
  ThreadPool();

  ThreadPool(const ThreadPool &other) = delete;
  ThreadPool(ThreadPool &&other) noexcept = delete;

  auto operator=(const ThreadPool &other) -> ThreadPool & = delete;
  auto operator=(ThreadPool &&other) noexcept -> ThreadPool & = delete;

  template <typename TaskType, typename ...Args>
  auto AssignTask(TaskType &&task, Args&& ...args)
    -> std::future<decltype(task(args...))> {

    using return_type = decltype(task(args...));
    auto packaged_task_ptr = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<TaskType>(task), std::forward<Args>(args)...)
    );

    auto future_obj = packaged_task_ptr->get_future();
    auto task_to_add = [packaged_task_ptr] {
      (*packaged_task_ptr)();
    };

    {
      auto lock = std::unique_lock<std::mutex>(pool_mutex_);
      tasks_.push(std::move(task_to_add));
    }
    condtion_to_allow_.notify_one();
    return future_obj;
  }

  auto Wait() noexcept -> void;

  ~ThreadPool();

private:
  static constexpr int8_t kNUMBER_OF_THREADS = 4;

  mutable std::mutex pool_mutex_;
  std::condition_variable condtion_to_allow_;
  std::atomic<bool> stop_workers_;

  std::queue<std::function<void()>> tasks_;
  std::vector<std::thread> workers_;

  auto StartWorker_() noexcept -> void;
  auto StopWorkersBase_() noexcept -> void;
};

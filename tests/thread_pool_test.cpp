#include <gtest/gtest.h>
#include "../include/thread_pool.hpp"

class ThreadPoolTest {
public:
  static auto constructors_test() -> void {
    auto pool = ThreadPool();
    EXPECT_EQ(pool.workers_.size(), pool.number_of_threads_);
    EXPECT_TRUE(pool.tasks_.empty());
    EXPECT_FALSE(pool.stop_workers_.load(std::memory_order_acquire));
  }

  static auto assign_task_test() -> void {
    auto pool = ThreadPool();
    auto n = 10;
    auto results = std::vector<std::future<int>>(n);

    for (int i = 0; i < n; i++) {
      results[i] = pool.AssignTask([i]{
        return i * 10;
      });
    }

    for (int i = 0; i < n; i++) {
      EXPECT_EQ(results[i].get(), i * 10);
    }
  }

  static auto shutdown_test() -> void {
    auto pool = ThreadPool();
    auto n = 10;
    auto results = std::vector<std::future<void>>{};

    for (int i = 0; i < n; i++) {
      results.push_back(pool.AssignTask([]{
        using namespace std::chrono;
        std::this_thread::sleep_for(microseconds(500));
      }));
    }

    pool.Shutdown();
    EXPECT_TRUE(pool.stop_workers_);
    EXPECT_EQ(results.size(), n);
  }

  static auto restart_test() -> void {
    auto pool = ThreadPool();

    constexpr int num_initial_tasks = 10;
    auto initial_task_ctr = std::atomic<int>(0);
    auto initial_futures = std::vector<std::future<void>>{};

    for (int i = 0; i < num_initial_tasks; i++) {
      initial_futures.emplace_back(pool.AssignTask([&initial_task_ctr]{
        initial_task_ctr++;
      }));
    }
    for (auto &ftr : initial_futures) ftr.get();
    pool.Shutdown();
    EXPECT_EQ(initial_task_ctr.load(), num_initial_tasks);

    pool.Restart();
    constexpr int restarted_initial_tasks = 10;
    auto restarted_task_ctr = std::atomic<int>(0);
    auto restarted_futures = std::vector<std::future<void>>{};

    for (int i = 0; i < restarted_initial_tasks; i++) {
      restarted_futures.emplace_back(pool.AssignTask([&restarted_task_ctr]{
        restarted_task_ctr++;
      }));
    }
    for (auto &ftr : restarted_futures) ftr.get();

    EXPECT_EQ(initial_task_ctr.load(), num_initial_tasks);
    EXPECT_EQ(restarted_task_ctr.load(), restarted_initial_tasks);
  }
};

TEST(ThreadPoolTest, ConstructorTest) {
  ThreadPoolTest::constructors_test();
}

TEST(ThreadPoolTest, AssignTaskTest) {
  ThreadPoolTest::assign_task_test();
}

TEST(ThreadPoolTest, ShutdownTest) {
  ThreadPoolTest::shutdown_test();
}

TEST(ThreadPoolTest, RestartTest) {
  ThreadPoolTest::restart_test();
}

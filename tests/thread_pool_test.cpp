#include <gtest/gtest.h>
#include "../include/thread_pool.hpp"

class ThreadPoolTest {
public:
  static auto constructors_test() -> void {
    auto pool = ThreadPool();
    EXPECT_EQ(pool.workers_.size(), pool.kNUMBER_OF_THREADS);
    EXPECT_TRUE(pool.tasks_.empty());
    EXPECT_FALSE(pool.stop_workers_);
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

  static auto stop_workers_test() -> void {
    auto pool = ThreadPool();
    auto n = 10;
    auto results = std::vector<std::future<void>>{};

    for (int i = 0; i < n; i++) {
      results.push_back(pool.AssignTask([]{
        using namespace std::chrono;
        std::this_thread::sleep_for(microseconds(500));
      }));
    }

    pool.Wait();
    EXPECT_TRUE(pool.stop_workers_);
    EXPECT_EQ(results.size(), n);
  }
};

TEST(ThreadPoolTest, ConstructorTest) {
  ThreadPoolTest::constructors_test();
}

TEST(ThreadPoolTest, AssignTaskTest) {
  ThreadPoolTest::assign_task_test();
}

TEST(ThreadPoolTest, StopWorkersTest) {
  ThreadPoolTest::stop_workers_test();
}

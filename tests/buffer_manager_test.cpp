#include <gtest/gtest.h>
#include "../include/buffer_manager.hpp"
#include "../include/thread_pool.hpp"
#include "../include/tick.hpp"
#include "../include/buffer.hpp"

class BufferManagerTest {
public:
  static auto simple_insert_test() -> void {
    auto pool = std::make_unique<ThreadPool>();
    auto manager = BufferManager(*pool);
    manager.maximum_sealed_buffers_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i < 3; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    EXPECT_EQ(manager.active_buffer_->Size(), 3);
    EXPECT_TRUE(manager.sealed_buffers_.load()->empty());
  }

  static auto trigger_sealing_test() -> void {
    auto pool = std::make_unique<ThreadPool>();
    auto manager = BufferManager(*pool);
    manager.maximum_buffer_size_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i <= 10; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    manager.pool_.Wait();

    auto sealed = manager.sealed_buffers_.load();
    EXPECT_EQ(sealed->size(), 2);
    EXPECT_EQ(manager.active_buffer_->Size(), 1);
  }

  static auto eviction_occurs() -> void {
    auto pool = std::make_unique<ThreadPool>();
    auto manager = BufferManager(*pool);
    manager.maximum_buffer_size_ = 2;
    manager.maximum_sealed_buffers_ = 2;

    for (int round = 0; round < 5; round++) {
      auto ticks = std::vector<Tick>{};
      for (int i = 0; i < 3; i++) {
        ticks.emplace_back(round * 100 + 1, 1.1 * i, 10 * i);
      }
      manager.Insert(ticks);
    }

    manager.pool_.Wait();
    auto sealed = manager.sealed_buffers_.load();
    EXPECT_LE(sealed->size(), 2);
  }
};

TEST(BufferManagerTest, SimpleInsertTest) {
  BufferManagerTest::simple_insert_test();
}

TEST(BufferManagerTest, TriggerSealingTest) {
  BufferManagerTest::trigger_sealing_test();
}

TEST(BufferManagerTest, EvictionOccursTest) {
  BufferManagerTest::eviction_occurs();
}

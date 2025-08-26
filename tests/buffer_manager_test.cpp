#include <gtest/gtest.h>

#include "../src/headers/buffer_manager.hpp"
#include "../src/headers/thread_pool.hpp"
#include "../src/headers/buffer.hpp"
#include "../include/bolt/tick.hpp"
#include "../src/headers/state.hpp"

namespace bolt {

class BufferManagerTest {
public:
  static auto simple_insert_test() -> void {
    auto pool = ThreadPool();
    auto manager = BufferManager(pool);
    manager.maximum_sealed_buffers_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i < 3; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    manager.pool_.Shutdown();
    EXPECT_EQ(manager.active_buffer_->Size(), 3);
    EXPECT_TRUE(manager.sealed_buffers_->empty());
  }

  static auto trigger_sealing_test() -> void {
    auto pool = ThreadPool();
    auto manager = BufferManager(pool);
    manager.maximum_buffer_size_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i <= 10; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    manager.pool_.Shutdown();

    auto sealed = manager.sealed_buffers_;
    EXPECT_EQ(sealed->size(), 2);
    EXPECT_EQ(manager.active_buffer_->Size(), 1);
  }

  static auto eviction_occurs() -> void {
    auto pool = ThreadPool();
    auto manager = BufferManager(pool);
    manager.maximum_buffer_size_ = 2;
    manager.maximum_sealed_buffers_ = 2;

    for (int round = 0; round < 5; round++) {
      auto ticks = std::vector<Tick>{};
      for (int i = 0; i < 3; i++) {
        ticks.emplace_back(round * 100 + 1, 1.1 * i, 10 * i);
      }
      manager.Insert(ticks);
    }

    manager.pool_.Shutdown();
    auto sealed = manager.sealed_buffers_;
    EXPECT_LE(sealed->size(), 2);
  }

  static auto get_state_test() -> void {
    auto pool = ThreadPool();
    auto manager = BufferManager(pool);
    manager.Insert({
      Tick(10, 1.1, 1),
      Tick(10, 1.1, 1),
      Tick(101, 1.1, 1)
    });
    manager.pool_.Shutdown();

    auto state = manager.GetState();
    EXPECT_EQ(state->GetActiveBuffer()->Size(), 3);
    EXPECT_EQ(state->GetActiveBuffer()->GetTimestamps().back(), 101);
  }
};

}

using namespace bolt;

TEST(BufferManagerTest, SimpleInsertTest) {
  BufferManagerTest::simple_insert_test();
}

TEST(BufferManagerTest, TriggerSealingTest) {
  BufferManagerTest::trigger_sealing_test();
}

TEST(BufferManagerTest, EvictionOccursTest) {
  BufferManagerTest::eviction_occurs();
}

TEST(BufferManagerTest, GetStateTest) {
  BufferManagerTest::get_state_test();
}

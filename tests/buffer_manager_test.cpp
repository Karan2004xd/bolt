#include <gtest/gtest.h>
#include "../include/buffer_manager.hpp"
#include "../include/thread_pool.hpp"
#include "../include/tick.hpp"
#include "../include/buffer.hpp"
#include "../include/state.hpp"

class BufferManagerTest : public ::testing::Test {
public:
  BufferManagerTest() {
    pool_ = std::make_unique<ThreadPool>();
  }

  static auto simple_insert_test() -> void {
    auto manager = BufferManager(*pool_);
    manager.maximum_sealed_buffers_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i < 3; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    EXPECT_EQ(manager.active_buffer_->Size(), 3);
    EXPECT_TRUE(manager.sealed_buffers_->empty());
  }

  static auto trigger_sealing_test() -> void {
    auto manager = BufferManager(*pool_);
    manager.maximum_buffer_size_ = 5;

    auto ticks = std::vector<Tick>{};
    for (int i = 0; i <= 10; i++) {
      ticks.emplace_back(100 * i, 1.1 * i, 10 * i);
    }

    manager.Insert(ticks);
    manager.pool_.Wait();

    auto sealed = manager.sealed_buffers_;
    EXPECT_EQ(sealed->size(), 2);
    EXPECT_EQ(manager.active_buffer_->Size(), 1);
  }

  static auto eviction_occurs() -> void {
    auto manager = BufferManager(*pool_);
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
    auto sealed = manager.sealed_buffers_;
    EXPECT_LE(sealed->size(), 2);
  }

  static auto get_state_test() -> void {
    auto manager = BufferManager(*pool_);
    manager.Insert({
      Tick(10, 1.1, 1)
    });

    auto state = manager.GetState();

    EXPECT_EQ(state->GetActiveBuffer()->Size(), 1);

    manager.Insert({
      Tick(10, 1.1, 1),
      Tick(101, 1.1, 1)
    });

    state = manager.GetState();
    EXPECT_EQ(state->GetActiveBuffer()->Size(), 3);
    EXPECT_EQ(state->GetActiveBuffer()->GetTimestamps().back(), 101);
  }

private:
  static std::unique_ptr<ThreadPool> pool_;
};

std::unique_ptr<ThreadPool> BufferManagerTest::pool_;

TEST_F(BufferManagerTest, SimpleInsertTest) {
  BufferManagerTest::simple_insert_test();
}

TEST_F(BufferManagerTest, TriggerSealingTest) {
  BufferManagerTest::trigger_sealing_test();
}

TEST_F(BufferManagerTest, EvictionOccursTest) {
  BufferManagerTest::eviction_occurs();
}

TEST_F(BufferManagerTest, GetStateTest) {
  BufferManagerTest::get_state_test();
}

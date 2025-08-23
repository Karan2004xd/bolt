#include <gtest/gtest.h>
#include "../include/buffer_manager.hpp"
#include "../include/thread_pool.hpp"

class BufferManagerTest {
public:
  static auto constructor_test() -> void {
    auto pool = std::make_unique<ThreadPool>();
    auto manager = BufferManager(*pool);
  }
};

TEST(BufferManagerTest, ConstructorTest) {
  BufferManagerTest::constructor_test();
}

#include <gtest/gtest.h>
#include "../include/ring_buffer.hpp"
#include "../include/tick.hpp"

class RingBufferTest {
public:
  static auto single_ingestion_test() -> void {
    auto ring_buffer = RingBuffer();
    auto n = 100;

    auto ticks = get_n_dummy_ticks_(n);
    auto read_ticks = std::vector<Tick>(n);

    auto producer = std::thread([&]{
      for (int i = 0; i < n; i++) {
        ring_buffer.Insert(ticks[i]);
      }
    });

    auto consumer = std::thread([&]{
      for (int i = 0; i < n; i++) {
        auto tick_opt = std::optional<Tick>();
        while (!(tick_opt = ring_buffer.Read())) {
          std::this_thread::yield();
        }
        auto tick = *tick_opt;
        read_ticks[i] = tick;
      }
    });

    producer.join();
    consumer.join();

    EXPECT_TRUE(read_ticks == ticks);
  }

  static auto empty_full_buffer_test() -> void {
    auto ring_buffer = RingBuffer();
    auto dummy_ticks = get_n_dummy_ticks_(ring_buffer.kBUFFER_SIZE_);

    // Check for full
    for (size_t i = 0; i < dummy_ticks.size(); i++) {
      if (i == dummy_ticks.size() - 1) {
        EXPECT_FALSE(ring_buffer.Insert(dummy_ticks[i]));
      } else {
        EXPECT_TRUE(ring_buffer.Insert(dummy_ticks[i]));
      }
    }

    // Check for empty
    for (size_t i = 0; i < dummy_ticks.size(); i++) {
      if (i == dummy_ticks.size() - 1) {
        EXPECT_FALSE(ring_buffer.Read());
      } else {
        EXPECT_TRUE(ring_buffer.Read());
      }
    }
  }

private:
  static auto get_n_dummy_ticks_(int n) -> std::vector<Tick> {
    if (n <= 0) return {};

    auto ticks = std::vector<Tick>(n);
    for (int i = 0; i < n; i++) {
      ticks[i] = Tick(10 * i, 1.217 * i, 1 * i);
    }
    return ticks;
  }
};

TEST(RingBufferTest, SingleIngestionTest) {
  RingBufferTest::single_ingestion_test();
}

TEST(RingBufferTest, EmptyFullBufferTest) {
  RingBufferTest::empty_full_buffer_test();
}

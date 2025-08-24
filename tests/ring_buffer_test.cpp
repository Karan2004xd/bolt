#include <gtest/gtest.h>
#include "../include/ring_buffer.hpp"
#include "../include/tick.hpp"

class RingBufferTest {
public:
  static auto single_ingestion_test() -> void {
    auto ring_buffer = RingBuffer();
    auto n = 100;

    auto ticks = get_n_dummy_ticks_(n, 10);
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
    auto n = 10;

    ring_buffer.ring_buffer_size_ = n;
    ring_buffer.buffer_.resize(n);

    auto dummy_ticks = get_n_dummy_ticks_(n, 10);

    // Check for full
    for (size_t i = 0; i <= dummy_ticks.size(); i++) {
      if (i == dummy_ticks.size()) {
        EXPECT_TRUE(ring_buffer.IsFull());
      } else {
        EXPECT_TRUE(ring_buffer.Insert(dummy_ticks[i]));
      }
    }

    // Check for empty
    for (size_t i = 0; i < dummy_ticks.size(); i++) {
      EXPECT_TRUE(ring_buffer.Read());
    }
    EXPECT_TRUE(ring_buffer.IsEmpty());
  }

  static auto scmp_test() -> void {
    auto ring_buffer = RingBuffer();

    const size_t n_producers = 3;
    const size_t items_per_producer = 500;
    const size_t total_items = n_producers * items_per_producer;

    auto dummy_ticks = std::vector<Tick>{};
    dummy_ticks.reserve(total_items);

    auto producers = std::vector<std::thread>{};
    for (size_t i = 0; i < n_producers; i++) {
      auto producer_ticks = get_n_dummy_ticks_(items_per_producer, i);
      dummy_ticks.insert(dummy_ticks.end(), producer_ticks.begin(), producer_ticks.end());

      producers.emplace_back([&ring_buffer, producer_ticks] {
        for (const auto &tick : producer_ticks) {
          while (!ring_buffer.Insert(tick)) {
            std::this_thread::yield();
          }
        }
      });
    }

    auto read_ticks = std::vector<Tick>{};
    read_ticks.reserve(total_items);

    auto consumer = std::thread([&]{
      for (size_t i = 0; i < total_items; i++) {
        auto tick_opt = std::optional<Tick>();
        while (!(tick_opt = ring_buffer.Read())) {
          std::this_thread::yield();
        }
        read_ticks.push_back(*tick_opt);
      }
    });

    for (auto &producer : producers) {
      if (producer.joinable()) {
        producer.join();
      }
    }

    consumer.join();

    ASSERT_EQ(read_ticks.size(), total_items);

    for (const auto &dummy_tick : dummy_ticks) {
      auto found = false;
      for (const auto &read_tick : read_ticks) {
        if (dummy_tick == read_tick) {
          found = true;
        }
      }
      EXPECT_TRUE(found);
    }
  }

private:
  static auto get_n_dummy_ticks_(int iterations, int multiple) -> std::vector<Tick> {
    if (iterations <= 0) return {};

    auto ticks = std::vector<Tick>(iterations);
    for (int i = 0; i < iterations; i++) {
      ticks[i] = Tick(10 * multiple + i,
                      1.1 * multiple + i,
                      1 * multiple + i);
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

TEST(RingBufferTest, SingleConsumerMultipleProducerTest) {
  RingBufferTest::scmp_test();
}

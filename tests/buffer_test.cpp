#include <gtest/gtest.h>
#include "../src/headers//buffer.hpp"
#include "../include/bolt/tick.hpp"

namespace bolt {

class BufferTest {
public:
  static auto constructors_test() -> void {
    auto ticks = std::vector<Tick> {
      Tick(1001, 100.01, 100, 1, 2, TradeConditions::kAcquisition),
      Tick(1002, 100.02, 101, 2, 3, TradeConditions::kCashSale),
      Tick(1003, 100.03, 102, 3, 4, TradeConditions::kRegularSale)
    };

    auto buffer = Buffer(
      {ticks[0], ticks[1], ticks[2]}
    );
    check_buffer_tick_equality_(buffer, ticks);

    auto buffer2 = Buffer(5);
    EXPECT_EQ(buffer2.timestamps_.capacity(), 5);
    EXPECT_EQ(buffer2.prices_.capacity(), 5);
    EXPECT_EQ(buffer2.volumes_.capacity(), 5);

    EXPECT_EQ(buffer2.symbol_ids_.capacity(), 5);
    EXPECT_EQ(buffer2.exchange_ids_.capacity(), 5);
    EXPECT_EQ(buffer2.trace_conditions_.capacity(), 5);
  }

  static auto semantics_test() -> void {
    auto ticks = std::vector<Tick> {
      Tick(1001, 100.01, 100, 1, 2, TradeConditions::kAcquisition),
      Tick(1002, 100.02, 101, 2, 3, TradeConditions::kCashSale),
    };

    // Copy semantics test
    auto buffer = Buffer({ ticks[0] });
    auto buffer2 = Buffer({ ticks[1] });

    auto buffer3 = Buffer(buffer);
    check_buffer_equality_(buffer, buffer3);

    buffer3 = buffer2;
    check_buffer_equality_(buffer3, buffer2);

    // Move semantics test
    buffer3 = buffer;
    buffer2 = Buffer(std::move(buffer));

    check_buffer_equality_(buffer3, buffer2);
    check_buffer_equality_(buffer, Buffer ());

    buffer3 = buffer2;
    buffer = std::move(buffer2);
    check_buffer_equality_(buffer3, buffer);
    check_buffer_equality_(buffer2, Buffer ());
  }

  static auto equality_test() -> void {
    auto buffer = Buffer({
      Tick(1001, 100.01, 100, 1, 2, TradeConditions::kAcquisition),
      Tick(1002, 100.02, 101, 2, 3, TradeConditions::kCashSale),
    });

    auto buffer2 = Buffer({
      Tick(1001, 100.01, 100, 1, 2, TradeConditions::kAcquisition),
    });

    EXPECT_FALSE(buffer == buffer2);
    EXPECT_TRUE(buffer != buffer2);
  }

  static auto getters_test() -> void {
    auto timestamps = std::vector<uint64_t>{1001, 1003, 1002};
    auto prices = std::vector<double>{100.001, 100.99, 100.02};
    auto volumes = std::vector<uint32_t>{100, 101, 102};

    auto symbol_ids = std::vector<uint32_t>{1, 2, 3};
    auto exchange_ids = std::vector<uint32_t>{2, 3, 4};
    auto trace_conditions = std::vector<TradeConditions>{
      TradeConditions::kRegularSale,
      TradeConditions::kCashSale,
      TradeConditions::kAcquisition
    };

    auto buffer = Buffer({
      Tick(timestamps[0], prices[0], volumes[0],
           symbol_ids[0], exchange_ids[0], trace_conditions[0]),

      Tick(timestamps[1], prices[1], volumes[1],
           symbol_ids[1], exchange_ids[1], trace_conditions[1]),

      Tick(timestamps[2], prices[2], volumes[2],
           symbol_ids[2], exchange_ids[2], trace_conditions[2]),
    });

    check_columns_equality_(buffer.timestamps_, buffer.GetTimestamps());
    check_columns_equality_(buffer.prices_, buffer.GetPrices());
    check_columns_equality_(buffer.volumes_, buffer.GetVolumes());

    check_columns_equality_(buffer.symbol_ids_, buffer.GetSymbolIds());
    check_columns_equality_(buffer.exchange_ids_, buffer.GetExchangeIds());
    check_columns_equality_(buffer.trace_conditions_, buffer.GetTraceCondtions());
  }

  static auto store_tick_test() -> void {
    auto buffer = Buffer();
    buffer.InsertTick(Tick(1001, 100.001, 100, 1, 2, TradeConditions::kCashSale));

    EXPECT_EQ(buffer.timestamps_.back(), 1001);
    EXPECT_DOUBLE_EQ(buffer.prices_.back(), 100.001);
    EXPECT_EQ(buffer.volumes_.back(), 100);

    EXPECT_EQ(buffer.symbol_ids_.back(), 1);
    EXPECT_EQ(buffer.exchange_ids_.back(), 2);
    EXPECT_EQ(buffer.trace_conditions_.back(), TradeConditions::kCashSale);
  }

  static auto sort_test() -> void {
    auto timestamps = std::vector<uint64_t>{1001, 1003, 1002};
    auto prices = std::vector<double>{100.001, 100.99, 100.02};
    auto volumes = std::vector<uint32_t>{100, 101, 102};

    auto symbol_ids = std::vector<uint32_t>{1, 2, 3};
    auto exchange_ids = std::vector<uint32_t>{2, 3, 4};
    auto trace_conditions = std::vector<TradeConditions>{
      TradeConditions::kRegularSale,
      TradeConditions::kCashSale,
      TradeConditions::kAcquisition
    };

    auto buffer = Buffer({
      Tick(timestamps[0], prices[0], volumes[0],
           symbol_ids[0], exchange_ids[0], trace_conditions[0]),

      Tick(timestamps[1], prices[1], volumes[1],
           symbol_ids[1], exchange_ids[1], trace_conditions[1]),

      Tick(timestamps[2], prices[2], volumes[2],
           symbol_ids[2], exchange_ids[2], trace_conditions[2]),
    });

    buffer.Sort();

    check_columns_equality_(buffer.timestamps_, {1001, 1002, 1003});
    check_columns_equality_(buffer.prices_, {100.001, 100.02, 100.99});
    check_columns_equality_(buffer.volumes_, {100, 102, 101});

    check_columns_equality_(buffer.symbol_ids_, {1, 3, 2});
    check_columns_equality_(buffer.exchange_ids_, {2, 4, 3});
    check_columns_equality_(buffer.trace_conditions_, {
      TradeConditions::kRegularSale,
      TradeConditions::kAcquisition,
      TradeConditions::kCashSale
    });

    buffer.Sort(false);
    check_columns_equality_(buffer.timestamps_, {1003, 1002, 1001});
    check_columns_equality_(buffer.prices_, {100.99, 100.02, 100.001});
    check_columns_equality_(buffer.volumes_, {101, 102, 100});

    check_columns_equality_(buffer.symbol_ids_, {2, 3, 1});
    check_columns_equality_(buffer.exchange_ids_, {3, 4, 2});
    check_columns_equality_(buffer.trace_conditions_, {
      TradeConditions::kCashSale,
      TradeConditions::kAcquisition,
      TradeConditions::kRegularSale
    });
  }

  static auto copy_test() -> void {
    auto buffer = Buffer({
      Tick(1001, 100.01, 100, 1, 2, TradeConditions::kAcquisition)
    });
    check_buffer_equality_(buffer, buffer.Copy());
  }

  static auto is_sorted_test() -> void {
    auto buffer = Buffer({
      Tick(1001, 100.01, 100),
      Tick(1002, 100.02, 101),
    });

    EXPECT_TRUE(buffer.IsSorted());

    buffer = Buffer({
      Tick(1001, 100.01, 100),
      Tick(1002, 100.02, 101),
      Tick(1000, 100.02, 101),
      Tick(1003, 100.02, 101)
    });

    EXPECT_FALSE(buffer.IsSorted());
  }

private:
  static auto check_buffer_tick_equality_(const Buffer &buffer,
                                          const std::vector<Tick> &ticks) -> void {
    ASSERT_EQ(buffer.timestamps_.size(), ticks.size());

    for (size_t i = 0; i < ticks.size(); i++) {
      EXPECT_EQ(buffer.timestamps_[i], ticks[i].GetTimestamp());
      EXPECT_EQ(buffer.volumes_[i], ticks[i].GetVolume());
      EXPECT_DOUBLE_EQ(buffer.prices_[i], ticks[i].GetPrice());

      EXPECT_EQ(buffer.symbol_ids_[i], ticks[i].GetSymbolId());
      EXPECT_EQ(buffer.exchange_ids_[i], ticks[i].GetExchangeId());
      EXPECT_EQ(buffer.trace_conditions_[i], ticks[i].GetTradeCondition());
    }
  }

  static auto check_buffer_equality_(const Buffer &buffer1,
                                     const Buffer &buffer2) -> void {
    ASSERT_NE(&buffer1, &buffer2);

    check_columns_equality_(buffer1.timestamps_, buffer2.timestamps_);
    check_columns_equality_(buffer1.prices_, buffer2.prices_);
    check_columns_equality_(buffer1.volumes_, buffer2.volumes_);

    check_columns_equality_(buffer1.symbol_ids_, buffer2.symbol_ids_);
    check_columns_equality_(buffer1.exchange_ids_, buffer2.exchange_ids_);
    check_columns_equality_(buffer1.trace_conditions_, buffer2.trace_conditions_);

    EXPECT_EQ(buffer1.is_sorted_, buffer2.is_sorted_);
  }

  template <typename T>
  static auto check_columns_equality_(const std::vector<T> &col1,
                                      const std::vector<T> &col2) -> void {
    EXPECT_TRUE(col1 == col2);
  }
};

}

using namespace bolt;

TEST(BufferTest, ConstructorsTest) {
  BufferTest::constructors_test();
}

TEST(BufferTest, CopyAndMoveSemanticsTest) {
  BufferTest::semantics_test();
}

TEST(BufferTest, EqualityOperatorTest) {
  BufferTest::equality_test();
}

TEST(BufferTest, GettersTest) {
  BufferTest::getters_test();
}

TEST(BufferTest, StoreTickTest) {
  BufferTest::store_tick_test();
}

TEST(BufferTest, SortTest) {
  BufferTest::sort_test();
}

TEST(BufferTest, CopyMethodTest) {
  BufferTest::copy_test();
}

TEST(BufferTest, IsSortedTest) {
  BufferTest::is_sorted_test();
}

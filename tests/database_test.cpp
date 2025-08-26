#include <gtest/gtest.h>
#include <algorithm>

#include "../include/bolt/database.hpp"
#include "../include/bolt/tick.hpp"
#include "../include/bolt/aggregate_result.hpp"

#include "../src/headers/buffer_manager.hpp"
#include "../src/headers/buffer.hpp"
#include "../src/headers/state.hpp"

namespace bolt {

class DatabaseTest {
public:
  static auto constructor_test() -> void {
    auto db = Database();

    EXPECT_TRUE(db.thread_pool_ != nullptr);
    EXPECT_TRUE(db.storage_handler_ != nullptr);
    EXPECT_TRUE(db.data_buffer_ != nullptr);
  }

  static auto insert_tick_test() -> void {
    auto db = Database();

    db.Insert(Tick(100, 1.1, 1));
    db.Insert(Tick(100, 1.1, 1));
    db.Flush();

    EXPECT_EQ(db.storage_handler_->GetState()->GetActiveBuffer()->Size(), 2);
  }

  static auto get_range_data_test() -> void {
    auto db = Database();
    db.Insert({
      Tick(100, 1.1, 1),
      Tick(101, 1.1, 2),
      Tick(102, 1.1, 3),
      Tick(99, 1.1, 4),
      Tick(100, 1.1, 5)
    });
    db.Flush();

    auto range_data = db.GetForRange(100, 103);
    EXPECT_EQ(range_data.size(), 4);
    EXPECT_EQ(range_data.front().GetTimestamp(), 100);
    EXPECT_EQ(range_data.back().GetTimestamp(), 102);

    range_data = db.GetForRange(99, 105, [](const Tick &tick) {
      return tick.GetVolume() >= 3;
    });

    EXPECT_EQ(range_data.size(), 3);
    EXPECT_EQ(range_data.front().GetTimestamp(), 99);
    EXPECT_EQ(range_data.back().GetTimestamp(), 102);
  }

  static auto sealed_buffers_range_test() -> void {
    auto db = Database();
    const auto &storage_handler = db.storage_handler_;

    // {100, 110, 120, 130, 140}
    auto sealed_buffer_1 = create_sorted_buffer(100, 10, 5);

    // {20, 30, 40, 50, 60}
    auto sealed_buffer_2 = create_sorted_buffer(20, 10, 5);

    // {125, 15, 80, 200, 95}
    auto active_buffer = std::make_shared<Buffer>();
    active_buffer->InsertTick(Tick(125, 1.0, 100));
    active_buffer->InsertTick(Tick(15, 1.0, 100));
    active_buffer->InsertTick(Tick(80, 1.0, 100));
    active_buffer->InsertTick(Tick(200, 1.0, 100));
    active_buffer->InsertTick(Tick(95, 1.0, 100));

    auto sealed_list_ptr = std::make_shared<State::sealed_list>();
    sealed_list_ptr->push_back(sealed_buffer_1);
    sealed_list_ptr->push_back(sealed_buffer_2);

    auto new_state = std::make_shared<const State>(active_buffer, sealed_list_ptr);
    storage_handler->current_state_.store(new_state);

    // {15, 20, 30, 40, 50, 60, 80, 95, 100, 110, 120, 125, 130, 140, 200}

    auto range_data = db.GetForRange(0, 150);

    EXPECT_EQ(range_data.size(), 14);
    EXPECT_EQ(range_data.front().GetTimestamp(), 15);
    EXPECT_EQ(range_data.back().GetTimestamp(), 140);

    EXPECT_TRUE(std::is_sorted(range_data.begin(), range_data.end(),
                               [](const Tick &a, const Tick &b){
                               return a.GetTimestamp() < b.GetTimestamp();
                               }));

    range_data = db.GetForRange(55, 115);

    EXPECT_EQ(range_data.size(), 5);
    EXPECT_EQ(range_data.front().GetTimestamp(), 60);
    EXPECT_EQ(range_data.back().GetTimestamp(), 110);

    EXPECT_TRUE(std::is_sorted(range_data.begin(), range_data.end(),
                               [](const Tick &a, const Tick &b){
                               return a.GetTimestamp() < b.GetTimestamp();
                               }));

    range_data = db.GetForRange(0, 100, [](const Tick &tick) {
      return tick.GetTimestamp() % 2 == 0;
    });

    EXPECT_EQ(range_data.size(), 7);
    EXPECT_EQ(range_data.front().GetTimestamp(), 20);
    EXPECT_EQ(range_data.back().GetTimestamp(), 100);
  }

  static auto size_test() -> void {
    auto db = Database();
    db.Insert({
      Tick(100, 1.1, 1),
      Tick(101, 1.1, 2),
      Tick(102, 1.1, 3),
      Tick(99, 1.1, 4),
      Tick(100, 1.1, 5)
    });
    db.Flush();
  }

  static auto aggregate_result_test() -> void {
    auto db = Database();
    db.Insert({
      Tick(100, 100.0, 10),
      Tick(101, 150.0, 20),
      Tick(102, 120.0, 30),
      Tick(103, 110.0, 60),
      Tick(104, 130.0, 0)
    });
    db.Flush();

    // Normal aggregation test
    auto result = db.Aggregate(100, 102);
    auto avg_price = (100.0 + 150.0 + 120.0) / 3;
    auto vwap = (100.0*10 + 150.0*20 + 120.0*30) / 60.0;
    
    auto expected_result = AggregateResult(3, 60, 150.0, 100.0,
                                           avg_price, vwap);
    EXPECT_TRUE(result == expected_result);

    // single tick range
    result = db.Aggregate(101, 101);
    expected_result = AggregateResult(1, 20, 150.0, 150.0, 150.0, 150.0);
    EXPECT_TRUE(result == expected_result);

    // empty range test
    result = db.Aggregate(200, 300);
    EXPECT_TRUE(result == AggregateResult());

    // zero volume vwap
    result = db.Aggregate(103, 104);
    EXPECT_EQ(result.GetCount(), 2);
    EXPECT_EQ(result.GetTotalVolume(), 60);
    EXPECT_DOUBLE_EQ(result.GetVwap(), (110.0*60 + 130.0*0) / 60.0);

    // Only a zero volume tick
    result = db.Aggregate(104, 104);
    EXPECT_EQ(result.GetCount(), 1);
    EXPECT_EQ(result.GetTotalVolume(), 0);
    EXPECT_DOUBLE_EQ(result.GetVwap(), 0.0);

    // filtered aggregation
    result = db.Aggregate(100, 105, [](const Tick &tick) {
      return tick.GetVolume() > 50;
    });

    EXPECT_EQ(result.GetCount(), 1);
    EXPECT_EQ(result.GetTotalVolume(), 60);
    EXPECT_EQ(result.GetAvgPrice(), 110.0);
    EXPECT_DOUBLE_EQ(result.GetVwap(), 110.0);
  }

private:
  static auto create_sorted_buffer(int64_t start_ts,
                                   int64_t step,
                                   size_t count) -> std::shared_ptr<Buffer> {

    auto buffer = std::make_shared<Buffer>();
    for (size_t i = 0; i < count; ++i) {
      buffer->InsertTick(Tick(start_ts + (i * step), 1.0, 100));
    }
    buffer->Sort();
    return buffer;
  }
};
}

using namespace bolt;

TEST(DatabaseTest, ConstructorsTest) {
  DatabaseTest::constructor_test();
}

TEST(DatabaseTest, InsertTickTest) {
  DatabaseTest::insert_tick_test();
}

TEST(DatabaseTest, GetRangeDataTest) {
  DatabaseTest::get_range_data_test();
}

TEST(DatabaseTest, SealedBuffersTest) {
  DatabaseTest::sealed_buffers_range_test();
}

TEST(DatabaseTest, SizeTest) {
  DatabaseTest::size_test();
}

TEST(DatabaseTest, AggregateResultTest) {
  DatabaseTest::aggregate_result_test();
}

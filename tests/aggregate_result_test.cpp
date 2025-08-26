#include <gtest/gtest.h>
#include "../include/aggregate_result.hpp"

class AggregateResultTest {
public:
  static auto constructor_test() -> void {
    auto obj = AggregateResult();

    EXPECT_EQ(obj.count_, {});
    EXPECT_EQ(obj.total_volume_, {});
    EXPECT_DOUBLE_EQ(obj.avg_price_, {});
    EXPECT_DOUBLE_EQ(obj.max_price_, {});
    EXPECT_DOUBLE_EQ(obj.min_price_, {});
    EXPECT_DOUBLE_EQ(obj.vwap_, {});

    obj = AggregateResult(100, 200, 100.0, 0, 1.1, 12.5);

    EXPECT_EQ(obj.count_, 100);
    EXPECT_EQ(obj.total_volume_, 200);
    EXPECT_DOUBLE_EQ(obj.avg_price_, 1.1);
    EXPECT_DOUBLE_EQ(obj.max_price_, 100.0);
    EXPECT_DOUBLE_EQ(obj.min_price_, 0);
    EXPECT_DOUBLE_EQ(obj.vwap_, 12.5);
  }

  static auto getter_test() -> void {
    auto obj = AggregateResult(100, 200, 100.0, 0, 1.1, 12.5);

    EXPECT_EQ(obj.GetCount(), 100);
    EXPECT_EQ(obj.GetTotalVolume(), 200);
    EXPECT_DOUBLE_EQ(obj.GetAvgPrice(), 1.1);
    EXPECT_DOUBLE_EQ(obj.GetMaxPrice(), 100.0);
    EXPECT_DOUBLE_EQ(obj.GetMinPrice(), 0);
    EXPECT_DOUBLE_EQ(obj.GetVwap(), 12.5);
  }

  static auto setter_test() -> void {
    auto obj = AggregateResult();

    obj.SetCount(100);
    obj.SetTotalVolume(200);
    obj.SetAvgPrice(1.1);
    obj.SetMaxPrice(100.0);
    obj.SetMinPrice(0);
    obj.SetVwap(12.5);

    EXPECT_EQ(obj.count_, 100);
    EXPECT_EQ(obj.total_volume_, 200);
    EXPECT_DOUBLE_EQ(obj.avg_price_, 1.1);
    EXPECT_DOUBLE_EQ(obj.max_price_, 100.0);
    EXPECT_DOUBLE_EQ(obj.min_price_, 0);
    EXPECT_DOUBLE_EQ(obj.vwap_, 12.5);
  }

  static auto equality_operator_test() -> void {
    auto obj = AggregateResult(100, 200, 100.0, 0, 1.1, 12.5);
    auto obj2 = obj;
    EXPECT_TRUE(obj2 == obj);

    obj.SetCount(101);
    EXPECT_TRUE(obj2 != obj);
  }
};

TEST(AggregateResultTest, ConstructorTest) {
  AggregateResultTest::constructor_test();
}

TEST(AggregateResultTest, GettersTest) {
  AggregateResultTest::getter_test();
}

TEST(AggregateResultTest, SettersTest) {
  AggregateResultTest::setter_test();
}

TEST(AggregateResultTest, EqualityOperatorTest) {
  AggregateResultTest::equality_operator_test();
}

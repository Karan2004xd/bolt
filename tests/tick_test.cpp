#include <gtest/gtest.h>
#include "../include/tick.hpp"

class TickTest {
public:
  static auto constructor_test() -> void {
    auto tick = Tick(1001, 101.223, 100);

    EXPECT_EQ(tick.timestamp_, 1001);
    EXPECT_DOUBLE_EQ(tick.price_, 101.223);
    EXPECT_EQ(tick.volume_, 100);
    EXPECT_EQ(tick.symbol_id_, {});
    EXPECT_EQ(tick.exchange_id_, {});
    EXPECT_EQ(tick.trace_condition_, TraceConditions::kNone);
  }

  static auto semantics_test() -> void {
    // Copy Semantics Test
    auto tick = Tick(1001, 1001.222347, 1000);
    auto tick2 = Tick(1002, 1001.22, 1001);
    auto tick3 = Tick(tick);
    check_objects_equality_(tick3, tick);

    tick3 = tick2;
    check_objects_equality_(tick2, tick3);

    // Move Semantics Test
    tick = Tick(1003, 1002.2332, 1003);
    tick2 = Tick(1004, 1003.2332, 1004);

    auto tick4 = tick;
    tick3 = Tick(std::move(tick));
    check_objects_equality_(tick3, tick4);

    tick4 = tick2;
    tick3 = std::move(tick2);
    check_objects_equality_(tick3, tick4);
  }

  static auto getters_test() -> void {
    auto tick = Tick(1001, 1001.122, 10010);
    tick.symbol_id_ = 1;
    tick.exchange_id_ = 2;
    tick.trace_condition_ = TraceConditions::kAcquisition;

    EXPECT_EQ(tick.GetTimestamp(), 1001);
    EXPECT_EQ(tick.GetSymbolId(), 1);
    EXPECT_EQ(tick.GetExchangeId(), 2);
    EXPECT_EQ(tick.GetVolume(), 10010);
    EXPECT_DOUBLE_EQ(tick.GetPrice(), 1001.122);
    EXPECT_EQ(tick.GetTraceCondtion(), TraceConditions::kAcquisition);
  }

  static auto setters_test() -> void {
    auto tick = Tick();

    tick.SetTimeStamp(1001);
    tick.SetPrice(1001.122);
    tick.SetVolume(10010);
    tick.SetSymbolId(1);
    tick.SetExchangeId(2);
    tick.SetTraceCondtition(TraceConditions::kAcquisition);

    EXPECT_EQ(tick.timestamp_, 1001);
    EXPECT_DOUBLE_EQ(tick.price_, 1001.122);
    EXPECT_EQ(tick.volume_, 10010);
    EXPECT_EQ(tick.symbol_id_, 1);
    EXPECT_EQ(tick.exchange_id_, 2);
    EXPECT_EQ(tick.trace_condition_, TraceConditions::kAcquisition);
  }

  static auto equality_operator_test() -> void {
    auto tick = Tick(1001, 100.223, 100);
    EXPECT_TRUE(tick == (Tick(1001, 100.223, 100)));
    EXPECT_TRUE(tick != (Tick(1001, 100.223, 300)));
  }

  static auto serialize_and_deserialize_test() -> void {
    auto tick = Tick(1001, 100.223, 100);

    char serialized_buffer[Tick::GetSerializedSize()];
    tick.Serialize(serialized_buffer);

    auto deserialized_tick = Tick();
    deserialized_tick.Deserialize(serialized_buffer);

    check_objects_equality_(deserialized_tick, tick);
  }

private:
  static auto check_objects_equality_(const Tick &tick1,
                                      const Tick &tick2) -> void {
    EXPECT_NE(&tick1, &tick2);
    EXPECT_EQ(tick1.timestamp_, tick2.timestamp_);
    EXPECT_EQ(tick1.symbol_id_, tick2.symbol_id_);
    EXPECT_EQ(tick1.exchange_id_, tick2.exchange_id_);
    EXPECT_DOUBLE_EQ(tick1.price_, tick2.price_);
    EXPECT_EQ(tick1.volume_, tick2.volume_);
    EXPECT_EQ(tick1.trace_condition_, tick2.trace_condition_);
  }
};

TEST(TickTest, ConstructorTest) {
  TickTest::constructor_test();
}

TEST(TickTest, CopyAndMoveSemanticsTest) {
  TickTest::semantics_test();
}

TEST(TickTest, GettersTest) {
  TickTest::getters_test();
}

TEST(TickTest, SettersTest) {
  TickTest::setters_test();
}

TEST(TickTest, EqualityOperatorCheck) {
  TickTest::equality_operator_test();
}

TEST(TickTest, SerializeAndDeserializeTest) {
  TickTest::serialize_and_deserialize_test();
}

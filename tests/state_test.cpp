#include <gtest/gtest.h>

#include "../src/headers/state.hpp"
#include "../src/headers/buffer.hpp"
#include "../include/bolt/tick.hpp"

namespace bolt {

class StateTest {
public:
  static auto constructors_test() -> void {
    auto sealed_buffers = State::sealed_list{
      std::make_shared<Buffer>(Buffer({
        Tick(1001, 10.001, 100, 1, 2, TradeConditions::kAutomaticExecution)
      })),

      std::make_shared<Buffer>(Buffer({
        Tick(1002, 10.001, 200, 1, 2, TradeConditions::kAcquisition)
      })),

      std::make_shared<Buffer>(Buffer({
        Tick(1003, 10.001, 300, 1, 2, TradeConditions::kCashSale)
      }))
    };

    auto active_buffer = Buffer({ Tick(1004, 100, 300) });

    auto state = State(
      std::make_shared<Buffer>(active_buffer.Copy()),
      std::make_shared<State::sealed_list>(sealed_buffers)
    );

    check_buffers_equality_(state, sealed_buffers);
    EXPECT_TRUE(active_buffer == *state.active_buffer_);
  }

  static auto semantics_test() -> void {
    auto sealed_buffer = State::sealed_list{
      std::make_shared<Buffer>(
        Buffer({Tick(100, 10.10, 10)})
      )
    };

    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State(
      std::make_shared<Buffer>(active_buffer),
      std::make_shared<State::sealed_list>(sealed_buffer)
    );
    sealed_buffer.push_back(sealed_buffer.back());

    auto state2 = State(
      std::make_shared<Buffer>(active_buffer),
      std::make_shared<State::sealed_list>(sealed_buffer)
    );

    // Copy Semantics Test
    auto state3 = State(state);
    EXPECT_TRUE(state3 == state);

    state3 = state2;
    EXPECT_TRUE(state3 == state2);

    // Move Semantics
    state3 = state;

    state2 = std::move(state);
    EXPECT_TRUE(state2 == state3);

    state3 = state2;

    state = std::move(state2);
    EXPECT_TRUE(state == state3);
  }

  static auto getters_test() -> void {
    auto sealed_buffer = State::sealed_list{
      std::make_shared<Buffer>(
        Buffer({Tick(100, 10.10, 10)})
      )
    };
    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State(
      std::make_shared<Buffer>(active_buffer.Copy()),
      std::make_shared<State::sealed_list>(sealed_buffer)
    );

    EXPECT_TRUE(state.active_buffer_ == state.GetActiveBuffer());
    EXPECT_TRUE(state.sealed_buffers_ == state.GetSealedBuffers());
  }

  static auto setters_test() -> void {
    auto sealed_buffer = State::sealed_list{
      std::make_shared<Buffer>(
        Buffer({Tick(100, 10.10, 10)})
      )
    };
    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State(
      std::make_shared<Buffer>(active_buffer.Copy()),
      std::make_shared<State::sealed_list>(sealed_buffer)
    );

    state.active_buffer_ = std::make_shared<Buffer>();
    EXPECT_FALSE(*state.active_buffer_ == active_buffer);

    state.sealed_buffers_ = std::make_shared<State::sealed_list>();
    EXPECT_EQ(state.sealed_buffers_->size(), 0);
  }

  static auto equality_operator_test() -> void {
    auto sealed_buffer = std::make_shared<State::sealed_list>(State::sealed_list{
      std::make_shared<Buffer>(
        Buffer({Tick(100, 10.10, 10)})
      )
    });
    auto active_buffer = std::make_shared<Buffer>(Buffer({Tick(101, 10.11, 20)}));

    auto state = State(active_buffer, sealed_buffer);
    auto state2 = State(active_buffer, sealed_buffer);

    EXPECT_TRUE(state == state2);
    EXPECT_FALSE(state != state2);

    state2.active_buffer_ = std::make_shared<Buffer>(Buffer({Tick(102, 1.1, 20)}));
    EXPECT_TRUE(state != state2);
    EXPECT_FALSE(state == state2);
  }

private:
  static auto check_buffers_equality_(const State &state,
                                      const State::sealed_list &buffers) -> void {
    ASSERT_EQ(state.sealed_buffers_->size(), buffers.size());

    for (size_t i = 0; i < buffers.size(); i++) {
      EXPECT_TRUE(buffers[i] == state.sealed_buffers_->at(i));
    }
  }
};
}

using namespace bolt;

TEST(StateTest, ConstructorsTest) {
  StateTest::constructors_test();
}

TEST(StateTest, CopyAndMoveSemanticsTest) {
  StateTest::semantics_test();
}

TEST(StateTest, GettersTest) {
  StateTest::getters_test();
}

TEST(StateTest, SettersTest) {
  StateTest::setters_test();
}

TEST(StateTest, EqualityOperatorTest) {
  StateTest::equality_operator_test();
}

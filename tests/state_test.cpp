#include <gtest/gtest.h>
#include "../include/state.hpp"
#include "../include/buffer.hpp"
#include "../include/tick.hpp"

class StateTest {
public:
  static auto constructors_test() -> void {
    auto sealed_buffers = State::sealed_list{
      std::make_shared<Buffer>(Buffer({
        Tick(1001, 10.001, 100, 1, 2, TraceConditions::kAutomaticExecution)
      })),

      std::make_shared<Buffer>(Buffer({
        Tick(1002, 10.001, 200, 1, 2, TraceConditions::kAcquisition)
      })),

      std::make_shared<Buffer>(Buffer({
        Tick(1003, 10.001, 300, 1, 2, TraceConditions::kCashSale)
      }))
    };

    auto active_buffer = Buffer({ Tick(1004, 100, 300) });

    auto state = State(std::make_shared<Buffer>(active_buffer.Copy()),
                       std::make_shared<State::sealed_list>(sealed_buffers));

    check_buffers_equality_(state, sealed_buffers);
    EXPECT_TRUE(active_buffer == *state.active_buffer_);
  }

  static auto semantics_test() -> void {
    auto sealed_buffer = Buffer({Tick(100, 10.10, 10)});
    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State({
      std::make_shared<Buffer>(sealed_buffer)
    }, std::make_shared<Buffer>(active_buffer.Copy()));

    auto state2 = State({
      std::make_shared<Buffer>(sealed_buffer),
      std::make_shared<Buffer>(sealed_buffer)
    }, std::make_shared<Buffer>(active_buffer.Copy()));

    // Copy Semantics Test
    auto state3 = State(state);
    check_state_equality_(state3, state);

    state3 = state2;
    check_state_equality_(state3, state2);

    // Move Semantics
    state3 = state;

    state2 = std::move(state);
    check_state_equality_(state2, state3);
    check_state_equality_(state, State());

    state3 = state2;

    state = std::move(state2);
    check_state_equality_(state, state3);
    check_state_equality_(state2, State());
  }

  static auto getters_test() -> void {
    auto sealed_buffer = Buffer({Tick(100, 10.10, 10)});
    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State({
      std::make_shared<Buffer>(sealed_buffer)
    }, std::make_shared<Buffer>(active_buffer.Copy()));

    EXPECT_TRUE(state.active_buffer_ == state.GetActiveBuffer());
    EXPECT_TRUE(state.sealed_buffers_ == state.GetSealedBuffers());
  }

  static auto setters_test() -> void {
    auto sealed_buffer = Buffer({Tick(100, 10.10, 10)});
    auto active_buffer = Buffer({Tick(101, 10.11, 20)});

    auto state = State({
      std::make_shared<Buffer>(sealed_buffer)
    }, std::make_shared<Buffer>(active_buffer.Copy()));

    state.SetActiveBuffer(std::make_shared<Buffer>());
    EXPECT_FALSE(*state.active_buffer_ == active_buffer);

    state.InsertSealedBuffer(std::make_shared<Buffer>(sealed_buffer));
    EXPECT_EQ(state.sealed_buffers_.size(), 2);
    EXPECT_TRUE(*state.sealed_buffers_.back() == sealed_buffer);
  }

private:
  static auto check_buffers_equality_(const State &state,
                                      const std::vector<Buffer> &buffers) -> void {
    ASSERT_EQ(state.sealed_buffers_.size(), buffers.size());

    for (size_t i = 0; i < buffers.size(); i++) {
      EXPECT_TRUE(buffers[i] == *state.sealed_buffers_[i]);
    }
  }

  static auto check_state_equality_(const State &state1,
                                    const State &state2) -> void {
    EXPECT_NE(&state1, &state2);
    EXPECT_TRUE(state1.active_buffer_ == state2.active_buffer_);
    EXPECT_TRUE(state1.sealed_buffers_ == state2.sealed_buffers_);
  }
};

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

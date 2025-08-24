#pragma once

#include "macros.hpp"
#include <deque>
#include <memory>

class Buffer;

class State {
  TEST_FRIEND(StateTest);

public:
  template <typename T>
  using ptr = std::shared_ptr<T>;

  using buffer = ptr<Buffer>;
  using sealed_list = std::deque<buffer>;

  State() = default;
  State(ptr<Buffer> active_buffer,
        const ptr<sealed_list> &sealed_buffers);

  State(const State &other);
  State(State &&other) noexcept;

  auto operator=(const State &other) -> State &;
  auto operator=(State &&other) noexcept -> State &;

  auto operator==(const State &other) const noexcept -> bool;
  auto operator!=(const State &other) const noexcept -> bool;

  auto SetSealedBuffer(const ptr<sealed_list> &sealed_buffers) noexcept -> void;
  auto SetActiveBuffer(std::shared_ptr<Buffer> active_buffer) noexcept -> void;

  auto GetSealedBuffers() const noexcept -> const ptr<const sealed_list> &;
  auto GetActiveBuffer() const noexcept -> const ptr<Buffer> &;

private:
  ptr<const sealed_list> sealed_buffers_;
  std::shared_ptr<Buffer> active_buffer_;

  auto CopyFrom_(const State &other) -> void;
  auto MoveFrom_(State &&other) noexcept -> void;
  auto EqualityCheck_(const State &other) const noexcept -> bool;
};

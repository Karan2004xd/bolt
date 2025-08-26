#pragma once

#include "../../include/bolt/macros.hpp"
#include <deque>
#include <memory>

namespace bolt {

class Buffer;

class State {
  TEST_FRIEND(StateTest);

public:
  template <typename T>
  using ptr = std::shared_ptr<T>;

  using buffer = ptr<Buffer>;
  using sealed_list = std::deque<buffer>;

  State();
  State(ptr<Buffer> active_buffer,
        const ptr<sealed_list> &sealed_buffers);

  State(const State &other);
  State(State &&other) noexcept;

  auto operator=(const State &other) -> State &;
  auto operator=(State &&other) noexcept -> State &;

  auto operator==(const State &other) const -> bool;
  auto operator!=(const State &other) const -> bool;

  auto GetSealedBuffers() const noexcept -> const ptr<const sealed_list> &;
  auto GetActiveBuffer() const noexcept -> const ptr<Buffer> &;

private:
  ptr<const sealed_list> sealed_buffers_;
  ptr<Buffer> active_buffer_;

  auto CopyFrom_(const State &other) -> void;
  auto MoveFrom_(State &&other) noexcept -> void;
  auto EqualityCheck_(const State &other) const -> bool;
};

}

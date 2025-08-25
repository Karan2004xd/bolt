#include "../include/state.hpp"
#include "../include/buffer.hpp"

State::State() {
  active_buffer_ = std::make_shared<Buffer>();
  sealed_buffers_ = std::make_shared<const sealed_list>();
}

State::State(ptr<Buffer> active_buffer,
             const ptr<sealed_list> &sealed_buffers) {
  active_buffer_ = std::move(active_buffer);
  sealed_buffers_ = sealed_buffers;
}

State::State(const State &other) {
  CopyFrom_(other);
}

State::State(State &&other) noexcept {
  MoveFrom_(std::move(other));
}

auto State::operator=(const State &other) -> State & {
  CopyFrom_(other);
  return *this;
}

auto State::operator=(State &&other) noexcept -> State & {
  MoveFrom_(std::move(other));
  return *this;
}

auto State::operator==(const State &other) const -> bool {
  return EqualityCheck_(other);
}

auto State::operator!=(const State &other) const -> bool {
  return !EqualityCheck_(other);
}

auto State::GetSealedBuffers() const noexcept
  -> const ptr<const sealed_list> & {
  return sealed_buffers_;
}

auto State::GetActiveBuffer() const noexcept -> const ptr<Buffer> & {
  return active_buffer_;
}

auto State::CopyFrom_(const State &other) -> void {
  sealed_buffers_ = other.sealed_buffers_;
  active_buffer_ = other.active_buffer_;
}

auto State::MoveFrom_(State &&other) noexcept -> void {
  sealed_buffers_ = std::move(other.sealed_buffers_);
  active_buffer_ = std::move(other.active_buffer_);
}

auto State::EqualityCheck_(const State &other) const -> bool {
  if (other.active_buffer_ != active_buffer_) return false;
  if (other.sealed_buffers_ != sealed_buffers_) return false;

  if (!active_buffer_ || !other.active_buffer_) return false;
  if (!sealed_buffers_ || !other.sealed_buffers_) return false;

  if (sealed_buffers_->size() != other.sealed_buffers_->size()) return false;
  for (size_t i = 0; i < sealed_buffers_->size(); i++) {
    if (*sealed_buffers_->at(i) != *other.sealed_buffers_->at(i)) {
      return false;
    }
  }
  return true;
}

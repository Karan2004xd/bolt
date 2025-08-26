#include "../include/database.hpp"
#include "../include/thread_pool.hpp"
#include "../include/ring_buffer.hpp"
#include "../include/buffer_manager.hpp"
#include "../include/tick.hpp"
#include "../include/buffer.hpp"
#include "../include/state.hpp"
#include "../include/constants.hpp"
#include "../include/aggregate_result.hpp"
#include <algorithm>

using namespace Constants;

Database::Database() {
  data_buffer_ = std::make_shared<RingBuffer>();
  thread_pool_ = std::make_shared<ThreadPool>();
  storage_handler_ = std::make_shared<BufferManager>(*thread_pool_);
  stop_insert_thread_ = false;

  StartInsertThread_();
}

Database::~Database() {
  stop_insert_thread_ = true;
  data_added_to_buffer_.notify_one();
  thread_pool_->Shutdown();
}

auto Database::Insert(const std::vector<Tick> &ticks) noexcept -> void {
  InsertBase_(ticks);
}

auto Database::Insert(const Tick &tick) noexcept -> void {
  InsertBase_({tick});
}

auto Database::GetForRange(uint64_t start_ts, uint64_t end_ts)
  -> std::vector<Tick> {

  if (start_ts > end_ts) return {};
  const auto &state = storage_handler_->GetState();

  return GetSortedTicks_(start_ts, end_ts, state);
}

auto Database::GetForRange(uint64_t start_ts,
                           uint64_t end_ts,
                           const filter_func &filter)
  -> std::vector<Tick> {

  if (start_ts > end_ts) return {};
  const auto &state = storage_handler_->GetState();

  return GetSortedTicks_(start_ts, end_ts, state, filter);
}

auto Database::Aggregate(uint64_t start_ts,
                         uint64_t end_ts) -> AggregateResult {

  if (start_ts > end_ts) return {};
  const auto &state = storage_handler_->GetState();

  auto sorted_ticks = GetSortedTicks_(start_ts, end_ts, state);
  auto result = AggregateResult();

  SetAggregateObj_(result, sorted_ticks);
  return result;
}

auto Database::Aggregate(uint64_t start_ts,
                         uint64_t end_ts,
                         const filter_func &filter) -> AggregateResult {

  if (start_ts > end_ts) return {};
  const auto &state = storage_handler_->GetState();

  auto sorted_ticks = GetSortedTicks_(start_ts, end_ts, state, filter);
  auto result = AggregateResult();

  SetAggregateObj_(result, sorted_ticks);
  return result;
}

auto Database::Size() const noexcept -> size_t {
  const auto &state = storage_handler_->GetState();

  auto curr_sealed_buffer_size = state->GetSealedBuffers()->size();
  if (curr_sealed_buffer_size > 0) {
    curr_sealed_buffer_size = (curr_sealed_buffer_size * ::kMAXIMUM_SEALED_BUFFER_SIZE);
    curr_sealed_buffer_size += state->GetSealedBuffers()->back()->Size();
  }
  return curr_sealed_buffer_size + state->GetActiveBuffer()->Size();
}

auto Database::Flush() noexcept -> void {
  while (!data_buffer_->IsEmpty()) {
    std::this_thread::yield();
  }
  stop_insert_thread_.store(true, std::memory_order_release);
  data_added_to_buffer_.notify_one();
  thread_pool_->Restart();
}

auto Database::GetTicksFromActiveBuffer_(
  const std::shared_ptr<const State> &state,
  uint64_t start_ts,
  uint64_t end_ts,
  const filter_func &filter) -> std::pair<bool, std::vector<Tick>> {

  const auto &active_buffer = state->GetActiveBuffer();
  auto ticks = std::vector<Tick>{};

  for (size_t i = 0; i < active_buffer->Size(); i++) {
    auto curr_ts = active_buffer->GetTimestamps().at(i);

    if (curr_ts >= start_ts && curr_ts <= end_ts) {
      CheckAndSetTick_(ticks, i, active_buffer, filter);
    }
  }

  return {active_buffer->IsSorted(), ticks};
}

auto Database::GetTicksFromSealedBuffer_(
  const std::shared_ptr<const State> &state,
  uint64_t start_ts,
  uint64_t end_ts,
  const filter_func &filter) -> std::pair<bool, std::vector<Tick>> {

  auto ticks = std::vector<Tick>{};
  auto sorted = true;
  const auto &sealed_buffers = state->GetSealedBuffers();

  if (!sealed_buffers->empty()) {
    for (const auto &buffer : *sealed_buffers) {
      const auto &ts_list = buffer->GetTimestamps();

      if (ts_list.empty()) continue;

      // No overlap
      if (end_ts < ts_list.front() || start_ts > ts_list.back()) {
        continue;
      }

      auto it_start = std::lower_bound(ts_list.begin(), ts_list.end(), start_ts);
      auto it_end = std::upper_bound(ts_list.begin(), ts_list.end(), end_ts);

      for (auto it = it_start; it != it_end; ++it) {
        size_t curr_idx = std::distance(ts_list.begin(), it);

        if (!ticks.empty() && sorted) {
          if (ticks.back().GetTimestamp() > buffer->GetTimestamps().at(curr_idx)) {
            sorted = false;
          }
        }
        CheckAndSetTick_(ticks, curr_idx, buffer, filter);
      }
    }
  }
  return {sorted, ticks};
}

auto Database::CheckAndSetTick_(std::vector<Tick> &ticks,
                                size_t index,
                                const std::shared_ptr<Buffer> &buffer,
                                const filter_func &filter) -> void {
  auto tick = Tick(
    buffer->GetTimestamps().at(index),
    buffer->GetPrices().at(index),
    buffer->GetVolumes().at(index),
    buffer->GetSymbolIds().at(index),
    buffer->GetExchangeIds().at(index),
    buffer->GetTraceCondtions().at(index)
  );

  if (filter(tick)) {
    ticks.emplace_back(std::move(tick));
  }
}

auto Database::GetSortedTicks_(
  uint64_t start_ts, uint64_t end_ts,
  const std::shared_ptr<const State> &state,
  const filter_func &filter) -> std::vector<Tick> {

  auto [sealed_ticks_sorted, sealed_ticks] =
    GetTicksFromSealedBuffer_(state, start_ts, end_ts, filter);

  auto [active_ticks_sorted, active_ticks] =
    GetTicksFromActiveBuffer_(state, start_ts, end_ts, filter);

  auto ticks = std::vector<Tick>();
  auto n = sealed_ticks.size() + active_ticks.size();
  ticks.reserve(n);

  if (active_ticks_sorted && sealed_ticks_sorted) {
    size_t i = 0, j = 0;
    while (i < active_ticks.size() && j < sealed_ticks.size()) {
      if (active_ticks[i].GetTimestamp() < sealed_ticks[j].GetTimestamp()) {
        ticks.push_back(std::move(active_ticks[i]));
        i++;
      } else {
        ticks.push_back(std::move(sealed_ticks[j]));
        j++;
      }
    }

    while (i < active_ticks.size()) {
      ticks.push_back(std::move(active_ticks[i]));
      i++;
    }

    while (j < sealed_ticks.size()) {
      ticks.push_back(std::move(sealed_ticks[j]));
      j++;
    }

  } else {
    ticks.insert(
      ticks.end(),
      std::make_move_iterator(sealed_ticks.begin()),
      std::make_move_iterator(sealed_ticks.end())
    );

    ticks.insert(
      ticks.end(),
      std::make_move_iterator(active_ticks.begin()),
      std::make_move_iterator(active_ticks.end())
    );

    std::sort(ticks.begin(), ticks.end(), [](const Tick &a, const Tick &b) {
      return a.GetTimestamp() < b.GetTimestamp();
    });
  }
  return ticks;
}

auto Database::StartInsertThread_() noexcept -> void {
  thread_pool_->AssignTask([this]{
    while (!stop_insert_thread_.load(std::memory_order_acquire)) {
      {
        auto lock = std::unique_lock<std::mutex>(insert_thread_mutex_);
        data_added_to_buffer_.wait(lock, [&]{
          return !data_buffer_->IsEmpty() ||
          stop_insert_thread_.load(std::memory_order_acquire); 
        });
      }

      if (!data_buffer_->IsEmpty() && !stop_insert_thread_) {
        auto tick_opt = std::optional<Tick>();
        while (!(tick_opt = data_buffer_->Read())) {
          std::this_thread::yield();
        }
        storage_handler_->Insert(*tick_opt);
      }
    }
  });
}

auto Database::InsertBase_(const std::vector<Tick> &ticks) noexcept -> void {
  for (const auto &tick : ticks) {
    data_buffer_->Insert(tick);
  }
  data_added_to_buffer_.notify_one();
}

auto Database::SetAggregateObj_(
  AggregateResult &result,
  const std::vector<Tick> &sorted_ticks) const noexcept -> void {

  auto at_start = true;

  for (const auto &tick : sorted_ticks) {
    result.SetTotalVolume(result.GetTotalVolume() + tick.GetVolume());
    result.SetAvgPrice(result.GetAvgPrice() + tick.GetPrice());

    result.SetVwap(result.GetVwap() + (tick.GetPrice() * tick.GetVolume()));

    if (at_start) {
      result.SetMinPrice(tick.GetPrice());
      result.SetMaxPrice(tick.GetPrice());
      at_start = false;

    } else {
      result.SetMaxPrice(std::max(result.GetMaxPrice(), tick.GetPrice()));
      result.SetMinPrice(std::min(result.GetMinPrice(), tick.GetPrice()));
    }

    result.SetCount(result.GetCount() + 1);
  }

  if (result.GetCount() > 0) {
    result.SetAvgPrice(result.GetAvgPrice() / result.GetCount());

    if (result.GetTotalVolume() > 0) {
      result.SetVwap(result.GetVwap() / result.GetTotalVolume());
    }
  }
}

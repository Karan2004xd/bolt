#include <benchmark/benchmark.h>
#include "../include/database.hpp"
#include "../include/constants.hpp"
#include "../include/tick.hpp"

class RangeQueryFixture : public benchmark::Fixture {
public:
  std::unique_ptr<Database> db;
  uint64_t total_ticks = 0;

  auto SetUp(const benchmark::State &state) -> void override {
    db = std::make_unique<Database>();

    int num_buffers_to_create = state.range(0);
    const auto ticks_per_buffer = Constants::kMAXIMUM_SEALED_BUFFER_SIZE;
    uint64_t current_ts = 1;

    for (int i = 0; i < num_buffers_to_create; i++) {
      auto batch = std::vector<Tick>{};
      batch.reserve(ticks_per_buffer);

      for (int j = 0; j < ticks_per_buffer; j++) {
        batch.emplace_back(current_ts++, 1.0, 100);
      }
      db->Insert(batch);
    }

    db->Flush();
    total_ticks = current_ts - 1;
  }

  auto TearDown(const benchmark::State &) -> void override {
  }
};

BENCHMARK_DEFINE_F(RangeQueryFixture, BM_QueryVsDBSize)(benchmark::State &state) {
  for (auto _ : state) {
    auto ticks = db->GetForRange(1, 5000);
    benchmark::DoNotOptimize(ticks);
  }
}

BENCHMARK_REGISTER_F(RangeQueryFixture, BM_QueryVsDBSize)
  ->Arg(1)->Arg(10)->Arg(50)->Arg(100);

BENCHMARK_DEFINE_F(RangeQueryFixture, BM_QueryVsRangeSize)(benchmark::State &state) {
  auto ticks_to_query = state.range(1);
  for (auto _ : state) {
    auto ticks = db->GetForRange(1, ticks_to_query);
    benchmark::DoNotOptimize(ticks);
  }
}

BENCHMARK_REGISTER_F(RangeQueryFixture, BM_QueryVsRangeSize)
  ->Args({50, 1000})
  ->Args({50, 10000})
  ->Args({50, 50000})
  ->Args({50, 100000});

BENCHMARK_DEFINE_F(RangeQueryFixture, BM_ReadWriteContention)(benchmark::State &state) {
  auto stop_iterator = std::atomic<bool>(false);

  auto inserter = std::thread([&]() {
    auto ts = total_ticks + 1;
    while (!stop_iterator.load()) {
      db->Insert(Tick(ts++, 1.0, 100));
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  for (auto _ : state) {
    auto ticks = db->GetForRange(1, 50000);
    benchmark::DoNotOptimize(ticks);
  }

  stop_iterator.store(true);
  if (inserter.joinable()) {
    inserter.join();
  }
}

BENCHMARK_REGISTER_F(RangeQueryFixture, BM_ReadWriteContention)->Arg(20);

// BENCHMARK_MAIN();

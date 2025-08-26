#include <benchmark/benchmark.h>
#include "../include/database.hpp"
#include "../include/constants.hpp"
#include "../include/tick.hpp"
#include "../include/aggregate_result.hpp"

class AggregateQueryFixture : public benchmark::Fixture {
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
};

BENCHMARK_DEFINE_F(AggregateQueryFixture,
                   BM_AggregateVsDbSize)(benchmark::State &state) {

  for (auto _ : state) {
    auto result = db->Aggregate(1, 5000);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK_DEFINE_F(AggregateQueryFixture,
                   BM_AggregateVsRangeSize)(benchmark::State &state) {

  uint64_t ticks_to_aggregate = state.range(1);
  for (auto _ : state) {
    auto result = db->Aggregate(1, ticks_to_aggregate);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK_DEFINE_F(AggregateQueryFixture,
                   BM_AggregateReadWriteContention)(benchmark::State &state) {

  auto stop_inserter = std::atomic<bool>(false);

  auto inserter = std::thread([&]{
    uint64_t ts = total_ticks + 1;
    while (!stop_inserter.load()) {
      db->Insert(Tick(ts++, 1.0, 100));
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  for (auto _ : state) {
    AggregateResult result = db->Aggregate(1, 50000);
    benchmark::DoNotOptimize(result);
  }

  stop_inserter.store(true);
  if (inserter.joinable()) {
    inserter.join();
  }
}

BENCHMARK_REGISTER_F(AggregateQueryFixture, BM_AggregateVsDbSize)
  ->Arg(1)->Arg(10)->Arg(50)->Arg(100);

BENCHMARK_REGISTER_F(AggregateQueryFixture, BM_AggregateVsRangeSize)
  ->Args({50, 1000})
  ->Args({50, 10000})
  ->Args({50, 50000})
  ->Args({50, 100000});

BENCHMARK_REGISTER_F(AggregateQueryFixture, BM_AggregateReadWriteContention)
  ->Arg(20);

// BENCHMARK_MAIN();

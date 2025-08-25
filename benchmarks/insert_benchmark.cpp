#include <benchmark/benchmark.h>
#include "../include/database.hpp"
#include "../include/tick.hpp"

static auto BM_SingleTickInsert(benchmark::State &state) -> void {
  auto db = Database();
  auto tick = Tick(101, 1.1, 1);

  for (auto _ : state) {
    db.Insert(tick);
  }

  state.SetItemsProcessed(state.iterations());
}

static auto BM_BatchTickInsert(benchmark::State &state) -> void {
  auto db = Database();
  const int batch_size = state.range(0);

  auto batch = std::vector<Tick>();
  batch.reserve(batch_size);

  for (int i = 0; i < batch_size; i++) {
    batch.emplace_back(100, 1.1, 1);
  }

  for (auto _ : state) {
    db.Insert(batch);
  }

  state.SetItemsProcessed(state.iterations() * batch_size);
}

BENCHMARK(BM_SingleTickInsert);
BENCHMARK(BM_BatchTickInsert)
  ->Arg(100)->Arg(1000)
  ->Arg(10000)->Arg(100000)
  ->Arg(1000000)->Arg(2000000)->Arg(3000000);

BENCHMARK_MAIN();

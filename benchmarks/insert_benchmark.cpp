#include <benchmark/benchmark.h>
#include "../include/bolt/database.hpp"
#include "../include/bolt/tick.hpp"

using namespace bolt;

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

static auto BM_AsyncTickInsert(benchmark::State &state) -> void {
  auto db = Database();
  const int batch_size = state.range(0);
  const int number_of_threads = state.range(1);

  auto batch = std::vector<Tick>();
  batch.reserve(batch_size);

  for (int i = 0; i < batch_size; i++) {
    batch.emplace_back(100000 - i, 1.1, 1);
  }

  for (auto _ : state) {
    auto threads = std::vector<std::thread>(number_of_threads);
    for (int i = 0; i < number_of_threads; i++) {
      threads[i] = std::thread([&]{
        db.Insert(batch);
      });
    }

    for (auto &thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  state.SetItemsProcessed(state.iterations() * batch_size);
}

BENCHMARK(BM_SingleTickInsert);
BENCHMARK(BM_BatchTickInsert)
  ->Arg(100)->Arg(1000)
  ->Arg(10000)->Arg(100000)
  ->Arg(1000000)->Arg(2000000)->Arg(3000000)
  ->Arg(10000000)->Arg(50000000);

BENCHMARK(BM_AsyncTickInsert)->Args({10000, 4});

BENCHMARK_MAIN();

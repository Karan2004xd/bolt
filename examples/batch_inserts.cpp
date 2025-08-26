#include <bolt/bolt.hpp>
#include <iostream>

using namespace bolt;

int main() {
  std::cout << "--- Bolt DB: Batch Insertion Example ---" << std::endl;
  Database db;

  // For high-throughput scenarios, it's best to insert in batches.
  // This amortizes the cost of notifying the consumer thread.
  std::vector<bolt::Tick> batch;
  for (int i = 0; i < 1000; ++i) {
    batch.emplace_back(2000 + i,
                       150.25 + (i * 0.01),
                       100 + i,
                       1, 2, TradeConditions::kBunchedTrade);
  }

  std::cout << "Inserting a batch of " << batch.size() << " ticks..." << std::endl;
  db.Insert(batch);

  // Get all insert operations
  db.Flush();

  // Verify the data was inserted
  auto results = db.GetForRange(2000, 3000);
  std::cout << "Query after batch insert found " << results.size() << " ticks." << std::endl;

  return 0;
}

#include <bolt/bolt.hpp>
#include <iostream>
#include <vector>

using namespace bolt;

int main() {
  std::cout << "--- Bolt DB: Advanced Query Example ---" << std::endl;
  bolt::Database db;

  // Insert data for two different symbols (ID 1 and ID 2)
  db.Insert({
    Tick(1000, 150.10, 100, 1, 1),
    Tick(1001, 150.12, 200, 1, 1),
    Tick(1002, 200.00, 50, 2, 1),
    Tick(1003, 150.08, 150, 1, 1),
    Tick(1004, 201.00, 70, 2, 1),
    Tick(1005, 150.11, 120, 1, 1)
  });

  // The Flush() method ensures all async insert operations are complete before querying.
  // This is useful for deterministic examples and tests.
  db.Flush();

  // 1. Filtered Query: Get all ticks for only symbol 1 using a lambda.
  auto symbol_1_filter = [](const bolt::Tick& tick) {
    return tick.GetSymbolId() == 1;
  };
  auto filtered_results = db.GetForRange(1000, 2000, symbol_1_filter);
  std::cout << "Filtered query for Symbol 1 found " << filtered_results.size() << " ticks." << std::endl;


  // 2. Aggregate Query: Get aggregate data for all trades of Symbol 2
  auto symbol_2_filter = [](const bolt::Tick& tick) {
    return tick.GetSymbolId() == 2;
  };
  bolt::AggregateResult agg_result = db.Aggregate(0, 3000, symbol_2_filter);

  std::cout << "\nAggregate results for Symbol 2:\n"
    << "  - Total Volume: " << agg_result.GetTotalVolume() << "\n"
    << "  - VWAP: " << agg_result.GetVwap() << "\n"
    << "  - High Price: " << agg_result.GetMaxPrice() << "\n"
    << "  - Low Price: " << agg_result.GetMinPrice() << std::endl;

  return 0;
}

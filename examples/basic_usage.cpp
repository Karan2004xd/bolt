#include <bolt/bolt.hpp>
#include <iostream>

using namespace bolt;

void PrintTicks(const std::string& title, const std::vector<bolt::Tick>& ticks) {
  std::cout << title << " (" << ticks.size() << " ticks found):\n";
  for (const auto& tick : ticks) {
    std::cout << "  - Timestamp: " << tick.GetTimestamp()
      << ", Price: " << tick.GetPrice()
      << ", Volume: " << tick.GetVolume() << std::endl;
  }
}

int main() {
  std::cout << "--- Bolt DB: Basic Usage Example ---" << std::endl;
  Database db;

  // Insert a single tick. This is a non-blocking, nanosecond-latency operation.
  db.Insert(Tick(1000, 150.25, 100, 1, 1, TradeConditions::kAcquisition)); // Symbol ID 1
  db.Insert(Tick(1001, 200.50, 50, 2, 1, TradeConditions::kAveragePriceTrade));  // Symbol ID 2

  // Get all insert operations
  db.Flush();

  // Perform a simple time-range query to get all ticks
  auto results = db.GetForRange(1000, 1001);
  PrintTicks("Querying for all symbols between timestamps 1000-1001", results);

  return 0;
}

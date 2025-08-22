#include <cstdint>

enum class TraceConditions : uint8_t {
  kNone,
  kRegularSale = 0,
  kAcquisition = 1,
  kAveragePriceTrade = 2,
  kAutomaticExecution = 3,
  kBunchedTrade = 4,
  kBunchedSoldTrade = 5,
  kCashSale = 6,
  kClosingPrints = 7,
  kCrossTrade = 8,
  kDerivativelyPriced = 9,
  // ... more to be added
};

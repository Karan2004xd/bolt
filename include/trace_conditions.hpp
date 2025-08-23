#pragma once
#include <cstdint>

enum class TraceConditions : uint8_t {
  kNone,
  kRegularSale = 1,
  kAcquisition = 2,
  kAveragePriceTrade = 3,
  kAutomaticExecution = 4,
  kBunchedTrade = 5,
  kBunchedSoldTrade = 6,
  kCashSale = 7,
  kClosingPrints = 8,
  kCrossTrade = 9,
  kDerivativelyPriced = 10,
  // ... more to be added
};

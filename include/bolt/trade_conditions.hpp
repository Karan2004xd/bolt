#pragma once
#include <cstdint>

/**
  * @file trace_conditions.hpp
  * @brief Contains majorly used Trade conditions enums, that can be useful for filtering
  *        evalutating ticks data.
  */

enum class TradeConditions : uint8_t {
    // --- General / Default ---
    kNone = 0,                      // No specific condition
    kRegularSale = 1,               // A standard trade without special conditions

    // --- Price-Related Conditions ---
    kAveragePriceTrade = 3,         // Price is an average of multiple trades
    kDerivativelyPriced = 10,       // Price is derived from another security or financial product
    kCashSale = 7,                  // Trade executed on a cash basis (not T+1/T+2)
    kNextDay = 12,                  // A trade that settles on the next business day
    kSeller = 13,                   // A seller-side only trade
    kPriorReferencePrice = 14,      // Price is based on a previous reference point
    kIntradayDetail = 15,           // Provides more detail on an intraday trade

    // --- Volume & Order-Related Conditions ---
    kAcquisition = 2,               // Part of an acquisition of a significant amount of stock
    kDistribution = 11,             // Part of a distribution of a significant amount of stock
    kBunchedTrade = 5,              // A single trade representing multiple smaller orders
    kBunchedSoldTrade = 6,          // A bunched trade that was a sell-side transaction
    kContingentTrade = 16,          // A trade dependent on other conditions being met
    kOddLotTrade = 17,              // A trade for a smaller than normal lot size (e.g., < 100 shares)
    kStoppedStock = 18,             // A trade where the price was guaranteed (stopped) for a period
    kVolumeWeightedAverage = 19,    // Trade executed at the volume-weighted average price (VWAP)

    // --- Execution Mechanism ---
    kAutomaticExecution = 4,        // Executed by an electronic, automatic system
    kCrossTrade = 9,                // A broker-dealer executes a buy and sell order for the same stock
    kIntermarketSweep = 20,         // An order that sweeps multiple exchanges to find liquidity
    kFormT = 21,                    // Out-of-sequence trade reported after market hours

    // --- Market Session Related ---
    kOpeningPrints = 22,            // Part of the market opening auction or prints
    kClosingPrints = 8,             // Part of the market closing auction or prints
    kOutOfSequence = 23,            // A trade reported late and out of time sequence
    kSoldLast = 24,                 // A late-reported sell-side trade
    kMarketCenterOfficialClose = 25,// The official closing price from a specific market center
    kMarketCenterOfficialOpen = 26, // The official opening price from a specific market center

    // --- Correction & Cancellation ---
    kCorrectedConsolidatedClose = 27, // A correction to the day's closing price
    kCancelled = 28                 // The trade was cancelled
};

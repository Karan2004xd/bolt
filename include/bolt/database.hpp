#pragma once
#include <condition_variable>
#include <atomic>
#include <functional>

#include "macros.hpp"

/**
* @file database.hpp
* @brief Defines the main entry point for the Bolt time-series database.
*/

namespace bolt {
class RingBuffer;
class ThreadPool;
class BufferManager;
class Tick;
class State;
class Buffer;
class AggregateResult;

/**
  * @class Database
  * @brief Manages the in-memory storage, retrieval and aggregation of Tick data.
  *
  * This class provides a thread-safe interface for inserting and querying time-series
  * data. All insertion operations are batched and handled by a background thread to
  * achieve high throughput.
  */
class Database {
  TEST_FRIEND(DatabaseTest);

public:
  using filter_func = std::function<bool(const Tick &)>;

  Database();

  Database(const Database &) = delete;
  Database(Database &&) = delete;

  auto operator=(const Database &) = delete;
  auto operator=(Database &&) = delete;

  /**
  * @brief Inserts a batch of ticks into the database.
  *
  * This method is the most efficient way to add data. The operation is
  * non-blocking and the data is processed asynchronously.
  *
  * @param ticks A vector of Tick objects to be stored.
  * @note This function is thread safe.
  */
  auto Insert(const std::vector<Tick> &ticks) noexcept -> void;

  /**
  * @brief Inserts a single tick object to database.
  *
  * This method inserts a single tick object to the database asynchronously.
  * This method is not recommended to be used for large quantity of data,
  * and using the batch inserts is recommended to get high performance.
  *
  * @param tick A single Tick object to store.
  * @note This function is thread safe.
  */
  auto Insert(const Tick &tick) noexcept -> void;

  /**
  * @brief Fetches all the data for the provided time range (inclusive).
  *
  * This method will fetch a range of data for a the provided range.
  *
  * The start of the fetched range will be either the start timestamp itself
  * or the a timestamp that is greater then that number.
  * Same is the case for the end, where the end will either will be end timestamp
  * provided or a timestamp smaller then that.
  *
  * @param start_ts The start of the time range (inclusive).
  * @param end_ts The end of the time range (inclusive).
  * @return A vector of Tick object.
  *
  * @note This function is thread safe.
  */
  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts) -> std::vector<Tick>;

  /**
  * @brief Fetches all the data for the provided time range (inclusive)
  *        with the feature to filter through the data fetched.
  *
  * This method will fetch a range of data for a the provided range.
  *
  * The start of the fetched range will be either the start timestamp itself
  * or the a timestamp that is greater then that number.
  * Same is the case for the end, where the end will either will be end timestamp
  * provided or a timestamp smaller then that.
  *
  * Each of the valid data object will be passed through the provided filter,
  * if returns true then only it is included in the result list or else it is
  * discarded.
  *
  * @param start_ts The start of the time range (inclusive).
  * @param end_ts The end of the time range (inclusive).
  * @param filter A callable type in which a Tick object can be passed and returns a boolean value.
  * @return A vector of Tick object.
  *
  * @note This function is thread safe.
  */
  auto GetForRange(uint64_t start_ts,
                   uint64_t end_ts,
                   const filter_func &filter) -> std::vector<Tick>;

  /**
  * @brief Provides useful and commonly used aggregate values.
  *
  * This method will calculate the aggregate values like
  * total volume, average prices, vwap etc, for the provided
  * start and end timestamp (inclusive).
  *
  * @param start_ts The start of the time range (inclusive).
  * @param end_ts The end of the time range (inclusive).
  * @return A object of AggregateResult.
  *
  * @note This function is thread safe.
  */
  auto Aggregate(uint64_t start_ts,
                 uint64_t end_ts) -> AggregateResult;

  /**
  * @brief Provides useful and commonly used aggregate values,
  *        with the feature to filter through the values.
  *
  * This method will calculate the aggregate values like
  * total volume, average prices, vwap etc, for the provided
  * start and end timestamp (inclusive).
  *
  * For the valid Tick object, it will passed through the filter
  * callable type and if returns then will be added to the result list
  * or else will be discarded.
  *
  * @param start_ts The start of the time range (inclusive).
  * @param end_ts The end of the time range (inclusive).
  * @param filter A callable type in which a Tick object can be passed and returns a boolean value.
  * @return A object of AggregateResult.
  *
  * @note This function is thread safe.
  */
  auto Aggregate(uint64_t start_ts,
                 uint64_t end_ts,
                 const filter_func &filter) -> AggregateResult;

  /**
  * @brief Provides the total number of data objects or rows currently present (in-memory).
  *
  * This is a simple utility method which will give the length currently
  * stored data in-memory.
  *
  * @return The total number of data object stored in-memory.
  */
  auto Size() const noexcept -> size_t;

  /**
  * @brief Makes sure that all the background threads have finished storing data
  *
  * This method will essentially will force the background threads to first
  * finish the ingestion task (if pending), then once done the thread pool is
  * refreshed to accept in new tasks.
  *
  * @note This is a blocking call, and will halt the ingestion task.
  */
  auto Flush() noexcept -> void;

  ~Database();

private:
  std::atomic<bool> stop_insert_thread_;

  mutable std::mutex insert_thread_mutex_;
  std::condition_variable data_added_to_buffer_;

  std::shared_ptr<RingBuffer> data_buffer_;
  std::shared_ptr<ThreadPool> thread_pool_;
  std::shared_ptr<BufferManager> storage_handler_;

  auto StartInsertThread_() noexcept -> void;
  auto InsertBase_(const std::vector<Tick> &ticks) noexcept -> void;

  auto GetTicksFromActiveBuffer_(
    const std::shared_ptr<const State> &state,
    uint64_t start_ts,
    uint64_t end_ts,
    const filter_func &filter = [](const Tick &){ return true;})
    -> std::pair<bool, std::vector<Tick>>;

  auto GetTicksFromSealedBuffer_(
    const std::shared_ptr<const State> &state,
    uint64_t start_ts,
    uint64_t end_ts,
    const filter_func &filter = [](const Tick &){ return true;})
    -> std::pair<bool, std::vector<Tick>>;

  auto CheckAndSetTick_(std::vector<Tick> &ticks,
                        size_t index,
                        const std::shared_ptr<Buffer> &buffer,
                        const filter_func &filter) -> void;

  auto GetSortedTicks_(
    uint64_t start_ts, uint64_t end_ts,
    const std::shared_ptr<const State> &state,
    const filter_func &filter = [](const Tick &){ return true;}) -> std::vector<Tick>;

  auto SetAggregateObj_(AggregateResult &result,
                        const std::vector<Tick> &sorted_ticks) const noexcept -> void;
};

}

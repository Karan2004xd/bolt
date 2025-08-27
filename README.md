# Bolt ⚡

**A high-throughput, low-latency in-memory time-series database for financial tick data, written in modern C++.**

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/Karan2004xd/bolt)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## Overview

Bolt is a specialized, performant time-series database built from the ground up for the unique demands of financial market data. It is designed to ingest and query massive volumes of tick-by-tick trade and quote data with nanosecond-level latency and throughput rates in the hundreds of millions of ticks per second.

The entire system is engineered around a lock-free, asynchronous architecture to maximize performance on multi-core systems.

## Performance Highlights

Benchmarks were conducted on a 4-core, 3.9 GHz CPU. The results demonstrate the system's capability to handle extreme data rates while maintaining low-latency query responses.

| Benchmark Scenario                       | Result                     | Latency     |
| :--------------------------------------- | :------------------------- | :---------- |
| **Multi-Producer Ingestion** (4 Threads) | **~294 Million ticks/sec** | -           |
| **Peak Single-Producer Ingestion**       | **~60 Million ticks/sec**  | -           |
| **Single Tick Ingestion Latency**        | -                          | **~133 ns** |
| **Range Query** (1M ticks in DB)         | -                          | **~1.4 ms** |
| **Aggregate Query** (OHLC, 1k ticks)     | -                          | **~433 µs** |


## Key Features

- **Lock-Free Ingestion Pipeline:** Utilizes a *lock-free* Multi-Producer, Single-Consumer (MPSC) ring buffer to decouple data ingestion from processing, enabling non-blocking inserts with nanosecond latency.
- **Columnar Storage Layout:** Tick data is stored in columns (SoA) rather than rows (AoS) to maximize CPU cache efficiency during analytical queries and scans.
- **Concurrent, Lock-Free Reads:** Readers access a consistent snapshot of the database via an atomic `shared_ptr`, allowing for completely non-blocking, thread-safe queries that do not interfere with high-speed writes.
- **Asynchronous Compaction & Sealing:** In-memory buffers are sealed, sorted, and published to readers by a background thread pool, keeping all expensive operations off the critical ingestion path.
- **Modern C++ Design:** Built with C++20, focusing on performance, safety, and modern idioms.


## Core Architectural Concepts

Bolt's performance is the result of three key design decisions:

1.  **Asynchronous & Lock-Free Ingestion:** All incoming ticks are written to a lock-free ring buffer and return to the caller in nanoseconds. A dedicated background thread consumes from this buffer, batches the data, and hands it off to the storage engine. This ensures the data producer is never blocked.

2.  **Columnar Storage:** Instead of storing `[Tick1, Tick2, Tick3]`, Bolt stores `[ts1, ts2, ts3]`, `[price1, price2, price3]`, etc. When a query only needs to analyze prices and volumes, it can read just those columns, leading to a massive reduction in memory bandwidth and dramatically improved CPU cache hit rates.

3.  **Atomic Snapshots for Reads:** Queries never operate on live, changing data. The background processing thread periodically publishes a new, immutable `State` object (containing the active buffer and all sealed, sorted buffers) via a single `std::atomic` operation. Readers simply load this atomic pointer to get a perfectly consistent and isolated view (*a copy*) of the entire database for the duration of their query.


## Architectural Scope & Trade-offs

Bolt `v1.0.0` is highly optimized for a specific set of use cases. The current design prioritizes maximum ingestion speed and in-memory query performance, leading to the following architectural choices:

- **In-Memory First By Design:** The primary goal is to serve as an extremely fast, memory-resident cache for hot, real-time data. Consequently, Bolt does not currently offer durability guarantees; data is lost upon application restart. A potential future direction is to add tiered storage for persisting sealed, cold buffers to disk.

- **Embedded Library, Not a Server:** To eliminate network overhead and provide the lowest possible latency, Bolt is designed as a library to be directly embedded within a C++ application. It does not have a network layer for remote clients, though this could be a feature for a future release.

- **Optimized For Time-Series Scans:** The query engine is specialized for its core competency: extremely fast scans over time ranges. It does not yet feature a complex query language, secondary indexing on non-timestamp columns, or support for joins.

- **Single-Writer Principle:** The database employs a single-consumer thread to process incoming data. This design choice simplifies the architecture, eliminates write-side lock contention, and ensures that data is processed in a predictable order, which is a powerful and common pattern in low-latency systems.


## Installation Guide & Requirements

### Requirements

Bolt is built using modern C++ features and requires a C++20 compliant compiler.

- **CMake** `3.16` or higher
- **C++20 Complaint Compiler**, such as:
	- GCC `10` or newer
	- Clang `10` or newer
	- MSVC `v19.29` (Visual Studio 2019 16.10) or newer
- **Google Test and Benchmark** (optional, only if you want to run the tests and benchmarks).

### Installation

 ```bash
# Clone the repository
git clone https://github.com/Karan2004xd/bolt.git
cd bolt

# Configure and build the project
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# On Linux: /usr/local/lib, /usr/local/include
# On Windows: C:/Program Files/bolt
sudo cmake --install build
```

### Using in Projects

- **Directly in Shell or Command Prompt**

```bash
# just this single include is required
g++ <name-of-file>.cpp -lbolt_lib
```

Here is a example CMake file demonstrating how you can use Bolt inside you projects

```bash
cmake_minimum_required(VERSION 3.16)
project(my_app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the installed Bolt library.
find_package(Bolt REQUIRED)

# Create your executable
add_executable(my_app main.cpp)

# Link your executable against the Bolt library.
# CMake handles all the include paths and library files automatically.
target_link_libraries(my_app PRIVATE Bolt::bolt_lib)
```

Now build and run,

```bash
cd my_app

# Make the build directory for CMake
cmake -B build

# Build your project
cd build && make

# Execute it!
./my_app
```

## API Usage (A Basic example)

Here is a simple example of how to use the Bolt database library.

```cpp
#include <bolt/bolt.hpp>
#include <iostream>
using namespace bolt;

int main() {
  Database db;

  // Insert a single tick with nanosecond latency
  db.Insert(Tick(1661434200000000000, 150.25, 100));

  // Insert a batch of ticks for maximum throughput
  std::vector<Tick> tick_batch = {
    {1661434200000000001, 150.26, 50},
    {1661434200000000002, 150.27, 200},
    {1661434199999999997, 150.27, 200}, // out of order data
    {1661434199999999996, 150.27, 200}
  };
  db.Insert(tick_batch);

  // Make sure all results are fetched
  db.Flush();

  // Query the database for a time range
  auto results = db.GetForRange(1661434199999999996, 1661434200000000002);

  for (const auto &result : results) {
    std::cout << result.GetTimestamp() << " ";
    std::cout << result.GetPrice() << " ";
    std::cout << result.GetVolume() << std::endl;
  }
  return 0;
}
```

**For more examples check out the `examples/` directory**

## Contributing & Future Work

Bolt is a new and actively developing project. While it has been tested thoroughly, there may still be bugs or areas for improvement. Community feedback, suggestions, and contributions are highly welcome and appreciated.

### How to Contribute

The best way to contribute is by opening an issue or submitting a pull request.

-   **Found a Bug?** If you've found a bug, please [open an issue](https://github.com/Karan2004xd/bolt/issues) and provide as much detail as possible, including steps to reproduce it.
-   **Have a Feature Idea?** I'd love to hear it! Please [open an issue](https://github.com/YOUR_USERNAME/bolt/issues) to discuss your idea.
-   **Want to Write Code?** If you'd like to contribute code, please follow this simple workflow:
    1.  Fork the repository.
    2.  Create a new feature branch (`git checkout -b feature/MyNewFeature`).
    3.  Commit your changes (`git commit -m 'Add some new feature'`).
    4.  Push to your branch (`git push origin feature/MyNewFeature`).
    5.  Open a Pull Request.

### Roadmap

Some of the potential features planned for future releases include:
-   Tiered storage (moving cold, in-memory buffers to disk).
-   Advanced secondary indexing on columns like `symbol_id`.
-   A network layer to allow remote clients to connect and query the database.

Thank you for your interest in Bolt!

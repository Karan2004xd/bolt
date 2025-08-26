project(benchmarks)

find_package(benchmark REQUIRED)
include_directories(benchmark_INCLUDE_DIRS)

set(BENCHMARK_SOURCES
  "./insert_benchmark.cpp"
  "./range_queries_benchmark.cpp"
  "./aggregate_queries_benchmark.cpp"
)

add_executable(${PROJECT_NAME} ${BENCHMARK_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  bolt_lib
  benchmark::benchmark
)

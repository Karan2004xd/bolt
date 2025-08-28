project(benchmarks)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.8.4
  GIT_PROGRESS ON
)
FetchContent_MakeAvailable(benchmark)

set(BENCHMARK_SOURCES
  "./insert_benchmark.cpp"
  "./range_queries_benchmark.cpp"
  "./aggregate_queries_benchmark.cpp"
)

add_executable(${PROJECT_NAME} ${BENCHMARK_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
  bolt_lib
  benchmark::benchmark_main
)

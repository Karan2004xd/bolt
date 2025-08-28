project(tests)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

set(TEST_SOURCES
  "./tick_test.cpp"
  "./ring_buffer_test.cpp"
  "./thread_pool_test.cpp"
  "./buffer_test.cpp"
  "./buffer_manager_test.cpp"
  "./state_test.cpp"
  "./database_test.cpp"
  "./aggregate_result_test.cpp"
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  bolt_lib
  GTest::gtest_main
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TESTING)
enable_testing()

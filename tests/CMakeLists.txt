project(tests)

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
  ${PROJECT_NAME} ${GTest_LIBRARIES}
  GTest::GTest GTest::Main
  bolt_lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TESTING)
enable_testing()

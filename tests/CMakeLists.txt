project(tests)

find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

set(TEST_SOURCES
  "./tick_test.cpp"
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(
  ${PROJECT_NAME} ${GTest_LIBRARIES}
  GTest::GTest GTest::Main src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UNIT_TESTING)
enable_testing()

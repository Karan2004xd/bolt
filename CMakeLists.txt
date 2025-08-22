cmake_minimum_required(VERSION 3.16)

project(bolt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# add_subdirectory("./src")
# add_subdirectory("./tests")

add_executable(${PROJECT_NAME} main.cpp)

# target_link_libraries(${PROJECT_NAME} PUBLIC src)

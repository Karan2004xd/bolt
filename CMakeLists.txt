cmake_minimum_required(VERSION 3.20)

project(bolt)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

set(CPACK_PACKAGE_VERSION "1.0.0")

add_subdirectory("./src")
add_subdirectory("./tests")
add_subdirectory("./benchmarks")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE bolt_lib)

# Packaging Rules
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 1)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
"Bolt: A high-performance time-series database library")
set(CPACK_PACKAGE_VENDOR "Karan Prajapati")
set(CPACK_GENERATOR "TGZ")

include(CPack)

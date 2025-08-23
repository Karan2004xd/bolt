project(src)

add_library(${PROJECT_NAME}
  "./tick.cpp"
  "./ring_buffer.cpp"
  "./thread_pool.cpp"
)

target_link_directories(
  ${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include"
)

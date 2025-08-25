project(bolt_lib)

add_library(${PROJECT_NAME}
  "./tick.cpp"
  "./ring_buffer.cpp"
  "./thread_pool.cpp"
  "./buffer.cpp"
  "./buffer_manager.cpp"
  "./state.cpp"
  "./database.cpp"
)

target_link_directories(
  ${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include"
)

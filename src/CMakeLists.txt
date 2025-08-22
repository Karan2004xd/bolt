project(src)

add_library(${PROJECT_NAME}
  "./tick.cpp"
)

target_link_directories(
  ${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include"
)

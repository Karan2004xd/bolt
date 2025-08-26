project(bolt_lib)

file(GLOB BOLT_SOURCES "*.cpp")

add_library(${PROJECT_NAME} ${BOLT_SOURCES})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    "${CMAKE_SOURCE_DIR}/include"

  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/headers"
)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/bolt"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

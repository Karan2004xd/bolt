project(bolt_lib)

# Find all source files
file(GLOB BOLT_SOURCES "*.cpp")

# Create the library
add_library(${PROJECT_NAME} ${BOLT_SOURCES})

# Specify public and private include directories
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/bolt>
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/headers"
)

# Install the library to the specified lib directory
# The EXPORT command creates a file that exports the target information
install(TARGETS ${PROJECT_NAME}
  EXPORT bolt_lib_targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install the public headers
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/bolt"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

# Generate and install the configuration files for find_package()
# This creates a BoltTargets.cmake file containing the exported target information
install(EXPORT bolt_lib_targets
  FILE BoltTargets.cmake
  NAMESPACE Bolt::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Bolt
)

# The configuration file that users will include
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/Bolt")
set(BOLT_VERSION "${CPACK_PACKAGE_VERSION}")

# Create the package configuration file for find_package(Bolt)
include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${CMAKE_SOURCE_DIR}/cmake/BoltConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/BoltConfig.cmake"
  INSTALL_DESTINATION "${CONFIG_INSTALL_DIR}"
)

# Create the version file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/BoltConfigVersion.cmake"
  VERSION "${BOLT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

# Install the generated config and version files
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/BoltConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/BoltConfigVersion.cmake"
  DESTINATION "${CONFIG_INSTALL_DIR}"
)

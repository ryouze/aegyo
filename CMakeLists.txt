# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.28)

# Set project name and language
project(aegyo LANGUAGES CXX)

# Set C++ standard to C++17, disable compiler-specific extensions and shared libraries
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project options
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_COMPILE_FLAGS "Enable compile flags" ON)

# Enforce out-of-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "[ERROR] In-source builds are not allowed. Use a separate build directory.")
endif()

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "[INFO] Defaulting to 'Release' build type.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Include external CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include custom modules
include(Flags)
include(External)

# Optionally enable global ccache
find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "[INFO] Ccache enabled for faster builds.")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message(WARNING "[WARNING] Ccache not found. Consider installing it to speed up rebuilds.")
endif()

# Get the project version using Git tags if available, else default to "unknown"
set(PROJECT_VERSION "unknown")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git REQUIRED)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(GIT_TAG)
    set(PROJECT_VERSION ${GIT_TAG})
    message(STATUS "[INFO] Project version set to ${PROJECT_VERSION} from Git.")
  else()
    message(WARNING "[WARNING] Failed to retrieve Git tag. Using fallback version: ${PROJECT_VERSION}.")
  endif()
else()
  message(WARNING "[WARNING] Git repository not found. Using fallback version: ${PROJECT_VERSION}.")
endif()

# Generate the version header using the inferred Git tag version
configure_file(${CMAKE_SOURCE_DIR}/src/version.hpp.in ${CMAKE_BINARY_DIR}/generated/version.hpp @ONLY)
include_directories(${CMAKE_BINARY_DIR}/generated)

# Add the main library target
add_library(${PROJECT_NAME}-lib STATIC
  src/app.cpp
  src/core/assets.cpp
  src/core/io.cpp
  src/core/rng.cpp
  src/core/string.cpp
  src/modules/vocabulary.cpp
)

# Include headers relatively to the src directory
target_include_directories(${PROJECT_NAME}-lib PUBLIC src)

# Apply public compile flags to the library target if enabled
if (ENABLE_COMPILE_FLAGS)
  apply_compile_flags(${PROJECT_NAME}-lib)
endif()

# Fetch and link external dependencies to the library target
fetch_and_link_external_dependencies(${PROJECT_NAME}-lib)

# Add the main executable and link the library (WIN32 is ignored on non-Windows platforms)
add_executable(${PROJECT_NAME} WIN32 src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-lib)

# If on macOS, bundle the executable into an app bundle
if(APPLE)
    # Set variables for Info.plist
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.ryouze.${PROJECT_NAME}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})

    # Generate the Info.plist file
    configure_file(${CMAKE_SOURCE_DIR}/cmake/assets/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)

    # Set macOS-specific properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
        INSTALL_RPATH "@executable_path/../Frameworks"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    # Copy the icon into the app bundle
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/cmake/assets/Icon.icns
            $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/Icon.icns
        COMMENT "Copying icon to the app bundle"
    )

    # Clean up the Frameworks directory before copying
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks
        COMMENT "Cleaning Frameworks directory"
    )

    # Copy only the SFML freetype framework into the app bundle
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND rsync -a ${SFML_SOURCE_DIR}/extlibs/libs-osx/Frameworks/freetype.framework
            $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Frameworks/
        COMMENT "Copying SFML freetype framework into the app bundle"
    )

    # Add install target for macOS app bundle
    install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION "/Applications")
elseif(WIN32)
    # Specify the path to the .ico file
    set(ICON_PATH "${CMAKE_SOURCE_DIR}/cmake/assets/Icon.ico")

    # Generate the icon.rc file dynamically
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/icon.rc" CONTENT "IDI_ICON1 ICON \"${ICON_PATH}\"")

    # Add the generated .rc file to the target
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/icon.rc")

    # Add install target for Windows executable
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
    # Add install target for regular executable
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# Add tests if enabled
if(BUILD_TESTS)
  # Enable testing with CTest
  enable_testing()

  # Add test executable
  add_executable(tests tests/test_all.cpp)
  target_link_libraries(tests PRIVATE ${PROJECT_NAME}-lib)

  # Define a function to register tests with CTest
  function(register_test test_name)
    add_test(NAME ${test_name} COMMAND tests ${test_name})
  endfunction()

  # Register tests using the function
  register_test("test_assets::load_font")
  register_test("test_rng::instance")
  register_test("test_rng::get_random_number")
  register_test("test_rng::get_random_bool")
  register_test("test_string::to_sfml_string")
  register_test("test_vocabulary::entry")
  register_test("test_vocabulary::category_count")

  message(STATUS "[INFO] Tests enabled.")
endif()

# Print the build type
message(STATUS "[INFO] Build type: ${CMAKE_BUILD_TYPE}.")

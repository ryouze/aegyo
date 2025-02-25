include(FetchContent)

function(fetch_and_link_external_dependencies target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Target '${target}' does not exist. Cannot fetch and link dependencies.")
  endif()

  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/deps")

  # SYSTEM is used to prevent applying compile flags to the dependencies
  FetchContent_Declare(
    fmt
    URL https://github.com/fmtlib/fmt/releases/download/11.1.3/fmt-11.1.3.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  # Do not build unnecessary SFML modules
  set(SFML_BUILD_AUDIO OFF)
  set(SFML_BUILD_NETWORK OFF)
  FetchContent_Declare(
    sfml
    URL https://github.com/SFML/SFML/archive/refs/tags/3.0.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  # Make dependencies available
  FetchContent_MakeAvailable(fmt sfml)

  # Link dependencies to the target
  target_link_libraries(${target} PUBLIC fmt::fmt SFML::Graphics)

  # Link SFML::Main for WIN32 targets to manage the WinMain entry point
  # This makes Windows use main() instead of WinMain(), so we can use the same entry point for all platforms
  if(WIN32)
    target_link_libraries(${target} PUBLIC SFML::Main)
  endif()

  message(STATUS "Linked dependencies 'fmt' and 'SFML' to target '${target}'.")
endfunction()

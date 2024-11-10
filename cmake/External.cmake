include(FetchContent)

function(fetch_and_link_external_dependencies target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "[ERROR] Target '${target}' does not exist. Cannot fetch and link dependencies.")
  endif()

  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
  set(FETCHCONTENT_QUIET OFF)
  set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/deps")

  # SYSTEM is used to prevent applying compile flags to the dependencies
  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        11.0.2
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  set(SFML_BUILD_AUDIO OFF)
  set(SFML_BUILD_NETWORK OFF)
  FetchContent_Declare(
    sfml
    URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.zip
    EXCLUDE_FROM_ALL
    SYSTEM
  )

  # Make dependencies available
  FetchContent_MakeAvailable(fmt sfml)

  # Link dependencies to the target
  target_link_libraries(${target} PUBLIC fmt::fmt sfml-graphics)

  # Link sfml-main for WIN32 targets to manage the WinMain entry point
  # This makes Windows use main() instead of WinMain(), so we can use the same entry point for all platforms
  if(WIN32)
    target_link_libraries(${target} PUBLIC sfml-main)
  endif()

  message(STATUS "[INFO] Linked dependencies 'fmt' and 'sfml' to target '${target}'.")
endfunction()

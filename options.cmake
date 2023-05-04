# bootstrap
option(BOOTSTRAP "Bootstrap dependencies and exit" OFF)
message(STATUS "Bootstrap mode: ${BOOTSTRAP}")

# ccache
if(NOT CMAKE_CXX_COMPILER_LAUNCHER)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  endif(CCACHE_PROGRAM)
endif()

# git info
set(GIT_RELEASE "" CACHE STRING "Override git release version")
set(GIT_COMMIT "" CACHE STRING "Override git commit hash")
if(NOT GIT_RELEASE OR NOT GIT_COMMIT)
  if(NOT Git_FOUND)
    find_package(Git QUIET)
    if(Git_FOUND)
      message(STATUS "Found git: ${GIT_EXECUTABLE}")
    endif()
  endif()
  if(Git_FOUND)
    if(NOT GIT_RELEASE)
      execute_process(
        RESULTS_VARIABLE GIT_RESULT
        OUTPUT_VARIABLE GIT_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        COMMAND sh -c "cd \"${PROJECT_SOURCE_DIR}\" && git describe --tags --abbrev=0"
      )
      if(GIT_RESULT EQUAL 0)
        string(REGEX MATCH "v?([0-9.]+)" RESULT ${GIT_RELEASE})
        set(GIT_RELEASE "${CMAKE_MATCH_1}")
      endif()
    endif()
    if(NOT GIT_COMMIT)
      execute_process(
        RESULTS_VARIABLE GIT_RESULT
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        COMMAND sh -c "cd \"${PROJECT_SOURCE_DIR}\" && git rev-parse HEAD"
      )
    endif()
  endif()
endif()
if(GIT_RELEASE)
  message(STATUS "Detected git release: ${GIT_RELEASE}")
else()
  set(GIT_RELEASE "0.0.0")
endif()
if(GIT_COMMIT)
  message(STATUS "Detected git commit: ${GIT_COMMIT}")
endif()

# git submodules
option(GIT_UPDATE "Update git submodules" ON)
if(GIT_UPDATE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  if(NOT Git_FOUND)
    find_package(Git QUIET)
    if(Git_FOUND)
      message(STATUS "Found git: ${GIT_EXECUTABLE}")
    endif()
  endif()
  if(Git_FOUND)
    message(STATUS "Updating git submodules")
    execute_process(
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE GIT_SUBMOD_RESULT
      COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    )
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "Failed to update git submodules")
    endif()
  endif()
endif()

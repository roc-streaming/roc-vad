# bootstrap
option(BOOTSTRAP "Bootstrap dependencies and exit" OFF)
message(STATUS "Bootstrap mode: ${BOOTSTRAP}")

# plugin address
set(PLUGIN_ADDRESS "127.0.0.1:9712"
  CACHE STRING "Specify plugin RPC address"
)

# plugin uuid
set(PLUGIN_UUID "FE352D6E-B65E-4496-BD3D-ECBBB6750E8B"
  CACHE STRING "Specify plugin UUID"
)

# plugin signature
set(CODESIGN_ID "" CACHE STRING "Specify Codesign ID")
if(NOT CODESIGN_ID)
  execute_process(COMMAND
    sh -c "security find-identity -v 2>/dev/null | grep 'Apple Development' | head -1 | awk '{print $2}'"
    OUTPUT_VARIABLE CODESIGN_ID
  )
  if(CODESIGN_ID)
    message(STATUS "Found Codesign ID: ${CODESIGN_ID}")
  endif()
endif()

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
set(GIT_TAG "" CACHE STRING "Override git tag")
if(NOT GIT_TAG)
  if(NOT Git_FOUND)
    find_package(Git QUIET)
    if(Git_FOUND)
      message(STATUS "Found git: ${GIT_EXECUTABLE}")
    endif()
  endif()
  if(Git_FOUND)
    execute_process(
      RESULTS_VARIABLE GIT_RESULT
      OUTPUT_VARIABLE GIT_COMMIT
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
      COMMAND sh -c "cd \"${PROJECT_SOURCE_DIR}\" && git rev-parse HEAD"
    )
    execute_process(
      RESULTS_VARIABLE GIT_RESULT
      OUTPUT_VARIABLE GIT_TAG
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
      COMMAND sh -c "cd \"${PROJECT_SOURCE_DIR}\" && git describe --tags --abbrev=0"
    )
    if(GIT_RESULT EQUAL 0)
      string(REGEX MATCH "v?([0-9.]+)" RESULT ${GIT_TAG})
      set(GIT_TAG "${CMAKE_MATCH_1}")
    endif()
  endif()
endif()
if(GIT_COMMIT)
  message(STATUS "Found git commit: ${GIT_COMMIT}")
endif()
if(GIT_TAG)
  message(STATUS "Found git tag: ${GIT_TAG}")
else()
  set(GIT_TAG "0.0.0")
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

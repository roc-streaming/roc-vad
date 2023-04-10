cmake_minimum_required(VERSION 3.13)

include(options.cmake)

project(roc_coreaudio_plugin CXX)

set(PROJECT_VERSION ${GIT_TAG})
set(PROJECT_COMMIT ${GIT_COMMIT})
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(COMMON_LIB "rcp_common")
set(PROTO_LIB "rcp_proto")

set(PLUGIN_NAME "rcp_plugin")
set(PLUGIN_BUNDLE "org.rocstreaming.coreaudioplugin")
set(PLUGIN_COPYRIGHT "Copyright (c) Roc Streaming authors")

set(TOOL_NAME "rcpadm")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_compile_options(
  # enable warnings
  "-Wall"
  "-Wextra"
  "-Wno-unused-parameter"

  # enable assertions (even in release)
  "-UNDEBUG"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(
    "-ggdb"
    "-funwind-tables"
    "-fno-omit-frame-pointer"
  )
endif()

include_directories(SYSTEM
  "common"
  "plugin/proto"
)

include(3rdparty/dependencies.cmake)

if(NOT BOOTSTRAP)
  add_subdirectory(common)
  add_subdirectory(plugin/proto)
  add_subdirectory(plugin)
  add_subdirectory(tool)

  add_dependencies(${PROTO_LIB}
    ${ALL_DEPENDENCIES}
  )

  foreach(TARGET IN ITEMS ${PLUGIN_NAME} ${TOOL_NAME})
    add_dependencies(${TARGET}
      ${ALL_DEPENDENCIES}
      ${PROTO_LIB}
    )
  endforeach()
endif()

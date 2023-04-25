cmake_minimum_required(VERSION 3.13)

include(options.cmake)

project(roc_vad CXX)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(COMMON_LIB "rocvad_common")
set(RPC_LIB "rocvad_rpc")

set(TOOL_NAME "roc-vad")

set(DRIVER_NAME "roc_vad")
set(DRIVER_BUNDLE_ID "org.rocstreaming.rocvad")
set(DRIVER_UUID "FE352D6E-B65E-4496-BD3D-ECBBB6750E8B")
set(DRIVER_SOCKET "127.0.0.1:9712")
set(DRIVER_MANUFACTURER "Roc Streaming")
set(DRIVER_COPYRIGHT "Copyright (c) Roc Streaming authors")

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

  # enable assertions (even in release build)
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
  "rpc"
)

include(3rdparty/dependencies.cmake)

if(NOT BOOTSTRAP)
  add_subdirectory(common)
  add_subdirectory(rpc)
  add_subdirectory(driver)
  add_subdirectory(tool)

  add_dependencies(${RPC_LIB}
    ${ALL_DEPENDENCIES}
  )

  foreach(TARGET IN ITEMS ${DRIVER_NAME} ${TOOL_NAME})
    add_dependencies(${TARGET}
      ${ALL_DEPENDENCIES}
      ${RPC_LIB}
    )
  endforeach()
endif()

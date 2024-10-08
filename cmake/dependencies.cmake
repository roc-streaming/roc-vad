include(ExternalProject)

include(ProcessorCount)
ProcessorCount(NUM_CPU)

set(LIST_SEPARATOR "!")
string(REPLACE ";" "${LIST_SEPARATOR}"
  OSX_ARCHITECTURES_LISTSEP "${CMAKE_OSX_ARCHITECTURES}")
string(REPLACE ";" ","
  OSX_ARCHITECTURES_COMMA "${CMAKE_OSX_ARCHITECTURES}")

if("$ENV{CI}" STREQUAL "1" OR "$ENV{CI}" STREQUAL "true")
  set(ENABLE_LOGS NO)
else()
  set(ENABLE_LOGS YES)
endif()

set(TOOL_LIST
  scons
  pkg-config
  autoconf
  automake
  libtool
)
foreach(TOOL IN LISTS TOOL_LIST)
  find_program(${TOOL}_EXE ${TOOL})
  if(NOT ${TOOL}_EXE)
    string(REPLACE ";" " " TOOL_LIST "${TOOL_LIST}")
    message(FATAL_ERROR
      "\nMissing required build tool: '${TOOL}'\nTry running:\nbrew install ${TOOL_LIST}\n")
  endif()
endforeach()

# Roc
set(SCONS_CMD
  scons -j ${NUM_CPU}
    -C ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/roc/src/roc_lib
    --prefix=${CMAKE_CURRENT_BINARY_DIR}/3rdparty/roc
    --enable-static
    --disable-shared
    --disable-tools
    --disable-sox
    --disable-openssl
    --build-3rdparty=all
    --compiler-launcher=${CMAKE_CXX_COMPILER_LAUNCHER}
    --macos-platform=${CMAKE_OSX_DEPLOYMENT_TARGET}
    --macos-arch=${OSX_ARCHITECTURES_COMMA}
)
ExternalProject_Add(roc_lib
  GIT_REPOSITORY "https://github.com/roc-streaming/roc-toolkit.git"
  GIT_TAG "master"
  GIT_SHALLOW OFF
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/roc
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ${SCONS_CMD}
  INSTALL_COMMAND ${SCONS_CMD} install
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/roc/include
)
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/roc/lib
)

# libASPL
ExternalProject_Add(aspl_lib
  GIT_REPOSITORY "https://github.com/gavv/libASPL.git"
  GIT_TAG "v3.1.1"
  GIT_SHALLOW OFF
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/aspl
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/aspl/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/aspl/lib/cmake
)

# BoringSSL
ExternalProject_Add(boringssl_lib
  GIT_REPOSITORY "https://github.com/google/boringssl.git"
  GIT_TAG "2db0eb3f96a5756298dcd7f9319e56a98585bd10"
  GIT_SHALLOW OFF
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/boringssl
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DOPENSSL_NO_ASM=1
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/boringssl/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/boringssl/lib/cmake
)

# gRPC
ExternalProject_Add(grpc_lib
  GIT_REPOSITORY "https://github.com/grpc/grpc.git"
  GIT_TAG "v1.63.0"
  GIT_SHALLOW ON
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/grpc
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DBUILD_TESTING=OFF
    -DgRPC_BUILD_TESTS=OFF
    -DgRPC_BUILD_CODEGEN=ON
    -DgRPC_BUILD_CSHARP_EXT=OFF
    -DgRPC_BUILD_GRPC_CPP_PLUGIN=ON
    -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF
    -DgRPC_SSL_PROVIDER=package
    -DOPENSSL_ROOT_DIR=${CMAKE_CURRENT_BINARY_DIR}/3rdparty/boringssl
    -DOPENSSL_USE_STATIC_LIBS=ON
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/grpc/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/grpc/lib/cmake
)
get_filename_component(GRPC_BIN_DIR
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/grpc/bin
  ABSOLUTE
)

# fmt
ExternalProject_Add(fmt_lib
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG "10.2.1"
  GIT_SHALLOW ON
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/fmt
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DBUILD_TESTING=OFF
    -DFMT_DOC=OFF
    -DFMT_INSTALL=ON
    -DFMT_TEST=OFF
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/fmt/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/fmt/lib/cmake
)

# spdlog
ExternalProject_Add(spdlog_lib
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.14.1"
  GIT_SHALLOW ON
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}/3rdparty/fmt/lib/cmake
    -DSPDLOG_FMT_EXTERNAL=ON
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog/lib/cmake
)
add_definitions(
  -DSPDLOG_COMPILED_LIB
  -DSPDLOG_FMT_EXTERNAL
)

# CLI111
ExternalProject_Add(cli11_lib
  GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
  GIT_TAG "v2.4.1"
  GIT_SHALLOW ON
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DBUILD_TESTING=OFF
    -DCLI11_BUILD_TESTS=OFF
    -DCLI11_BUILD_EXAMPLES=OFF
    -DCLI11_SINGLE_FILE=ON
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11/include
)

# GoogleTest
ExternalProject_Add(googletest_lib
  GIT_REPOSITORY "https://github.com/google/googletest.git"
  GIT_TAG "v1.14.0"
  GIT_SHALLOW ON
  GIT_PROGRESS ON
  UPDATE_DISCONNECTED ON
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/googletest
  LIST_SEPARATOR ${LIST_SEPARATOR}
  CMAKE_ARGS
    -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
    -DCMAKE_OSX_ARCHITECTURES=${OSX_ARCHITECTURES_LISTSEP}
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DBUILD_TESTING=OFF
  BUILD_COMMAND
    ${CMAKE_COMMAND} --build . -- -j ${NUM_CPU}
  LOG_DOWNLOAD ${ENABLE_LOGS}
  LOG_CONFIGURE ${ENABLE_LOGS}
  LOG_BUILD ${ENABLE_LOGS}
  LOG_INSTALL ${ENABLE_LOGS}
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/googletest/include
)
list(PREPEND CMAKE_PREFIX_PATH
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/googletest/lib/cmake
)

# List of all third-party dependencies
set(ALL_DEPENDENCIES
  roc_lib
  aspl_lib
  boringssl_lib
  grpc_lib
  fmt_lib
  spdlog_lib
  cli11_lib
  googletest_lib
)

# Serialize dependencies
# (each one depends on previous in list)
set(REV_DEPENDENCIES ${ALL_DEPENDENCIES})
list(REVERSE REV_DEPENDENCIES)
set(OTHER_DEPENDENCIES ${ALL_DEPENDENCIES})
foreach(DEPENDENCY IN LISTS REV_DEPENDENCIES)
  list(REMOVE_ITEM OTHER_DEPENDENCIES ${DEPENDENCY})
  if(OTHER_DEPENDENCIES)
    add_dependencies(${DEPENDENCY}
      ${OTHER_DEPENDENCIES}
    )
  endif()
endforeach()

# After building dependencies, touch commit-file
add_custom_command(
  COMMENT "Commit bootstrap"
  DEPENDS ${ALL_DEPENDENCIES}
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/bootstrap.commit
  COMMAND ${CMAKE_COMMAND} -E touch
    ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/bootstrap.commit
)
add_custom_target(commit_bootstrap ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/bootstrap.commit
)

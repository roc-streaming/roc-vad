find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
)

add_executable(${TEST_EXE}
  main.cpp
  test_format_parse.cpp
  test_index_allocator.cpp
  test_ring_buffer.cpp
  test_uid_generator.cpp
)

target_link_libraries(${TEST_EXE} PRIVATE
  # internal libs
  ${DRIVER_LIB}
  ${TOOL_LIB}
  ${COMMON_LIB}
  ${RPC_LIB}

  # third-party libs
  gRPC::grpc++_unsecure
  spdlog::spdlog
  fmt::fmt
  GTest::gtest
)

add_test(
  NAME ${TEST_EXE}
  COMMAND ${TEST_EXE}
)

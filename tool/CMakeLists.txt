find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(${TOOL_NAME}
  "cmd_base.cpp"
  "cmd_compound.cpp"
  "cmd_device.cpp"
  "cmd_device_add.cpp"
  "cmd_info.cpp"
  "cmd_root.cpp"
  "connector.cpp"
  "main.cpp"
)

target_link_libraries(${TOOL_NAME} PRIVATE
  ${COMMON_LIB}
  ${PROTO_LIB}
)

target_link_libraries(${TOOL_NAME} PRIVATE
  gRPC::grpc++_unsecure
  spdlog::spdlog
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_link_options(${TOOL_NAME} PRIVATE
    "-Wl,-unexported_symbol,*"
  )
  add_custom_command(TARGET ${TOOL_NAME} POST_BUILD
    COMMENT "Stripping ${TOOL_NAME}"
    COMMAND strip $<TARGET_FILE:${TOOL_NAME}>
  )
endif()

add_custom_command(TARGET ${TOOL_NAME} POST_BUILD
  COMMENT "Copying ${TOOL_NAME} to bin"
  COMMAND mkdir -p
    ${PROJECT_ROOT}/bin
  COMMAND cp
    $<TARGET_FILE:${TOOL_NAME}>
    ${PROJECT_ROOT}/bin
)

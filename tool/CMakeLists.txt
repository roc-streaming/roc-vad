find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_library(LIB_Foundation Foundation REQUIRED)

add_library(${TOOL_LIB} STATIC
  cmd_base.cpp
  cmd_compound.cpp
  cmd_device.cpp
  cmd_device_add.cpp
  cmd_device_add_receiver.cpp
  cmd_device_add_sender.cpp
  cmd_device_bind.cpp
  cmd_device_connect.cpp
  cmd_device_delete.cpp
  cmd_device_disable.cpp
  cmd_device_enable.cpp
  cmd_device_list.cpp
  cmd_device_show.cpp
  cmd_info.cpp
  cmd_logcat.cpp
  cmd_root.cpp
  cmd_uninstall.cpp
  connector.cpp
  format.cpp
  help_formatter.cpp
  parse.cpp
  print.cpp
)

add_executable(${TOOL_EXE}
  main.cpp
)

target_link_libraries(${TOOL_EXE} PRIVATE
  # internal libs
  ${TOOL_LIB}
  ${COMMON_LIB}
  ${RPC_LIB}

  # third-party libs
  gRPC::grpc++_unsecure
  spdlog::spdlog
  fmt::fmt

  # system libs
  ${LIB_Foundation}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_link_options(${TOOL_EXE} PRIVATE
    # hide all symbols, this significantly reduces binary size
    "-Wl,-unexported_symbol,*"
  )
  add_custom_command(TARGET ${TOOL_EXE} POST_BUILD
    COMMENT "Stripping ${TOOL_EXE}"
    COMMAND strip $<TARGET_FILE:${TOOL_EXE}>
  )
endif()

add_custom_command(TARGET ${TOOL_EXE} POST_BUILD
  COMMENT "Copying ${TOOL_EXE} to bin"
  COMMAND ${CMAKE_COMMAND} -E make_directory
    ${PROJECT_ROOT}/bin
  COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:${TOOL_EXE}>
    ${PROJECT_ROOT}/bin
)

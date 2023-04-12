find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(libASPL CONFIG REQUIRED)
find_library(LIB_CoreFoundation CoreFoundation REQUIRED)

add_library(${DRIVER_NAME} MODULE
  "device.cpp"
  "device_manager.cpp"
  "driver.cpp"
  "driver_service.cpp"
  "entry_point.cpp"
  "log_manager.cpp"
  "log_sender.cpp"
  "log_syslog.cpp"
)

target_link_libraries(${DRIVER_NAME} PRIVATE
  # internal libs
  ${COMMON_LIB}
  ${RPC_LIB}

  # third-party libs
  gRPC::grpc++_unsecure
  spdlog::spdlog
  aspl::libASPL

  # system libs
  ${LIB_CoreFoundation}
)

target_link_options(${DRIVER_NAME} PRIVATE
  # hide everything except entry_point()
  "-Wl,-exported_symbol,_entry_point"
)

set_target_properties(${DRIVER_NAME} PROPERTIES
  OUTPUT_NAME "${DRIVER_NAME}"
  BUNDLE true
  BUNDLE_EXTENSION "driver"
  PREFIX ""
  SUFFIX ""
  MACOSX_BUNDLE ON
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
  MACOSX_BUNDLE_BUNDLE_NAME "${DRIVER_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
  MACOSX_BUNDLE_COPYRIGHT "${DRIVER_COPYRIGHT}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "${DRIVER_BUNDLE}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_custom_command(TARGET ${DRIVER_NAME} POST_BUILD
    COMMENT "Stripping ${DRIVER_NAME}"
    COMMAND
      strip -x $<TARGET_FILE:${DRIVER_NAME}>
  )
endif()

if(NOT CODESIGN_ID STREQUAL "")
  add_custom_command(TARGET ${DRIVER_NAME} POST_BUILD
    COMMENT "Signing ${DRIVER_NAME}.driver with ID ${CODESIGN_ID}"
    VERBATIM
    COMMAND
      codesign --force -s ${CODESIGN_ID}
        ${CMAKE_CURRENT_BINARY_DIR}/${DRIVER_NAME}.driver
      )
endif()

add_custom_command(TARGET ${DRIVER_NAME} POST_BUILD
  COMMENT "Copying ${DRIVER_NAME} to bin"
  COMMAND mkdir -p
    ${PROJECT_ROOT}/bin
  COMMAND rm -rf
    ${PROJECT_ROOT}/bin/${DRIVER_NAME}.driver
  COMMAND cp -a
    ${CMAKE_CURRENT_BINARY_DIR}/${DRIVER_NAME}.driver
    ${PROJECT_ROOT}/bin
)

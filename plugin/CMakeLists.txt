find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(libASPL CONFIG REQUIRED)
find_library(LIB_CoreFoundation CoreFoundation REQUIRED)

add_library(${PLUGIN_NAME} MODULE
  "device.cpp"
  "device_manager.cpp"
  "driver.cpp"
  "entry_point.cpp"
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  ${COMMON_LIB}
  ${PROTO_LIB}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
  gRPC::grpc++_unsecure
  aspl::libASPL
  ${LIB_CoreFoundation}
)

target_link_options(${PLUGIN_NAME} PRIVATE
  "-Wl,-exported_symbol,_entry_point"
)

set_target_properties(${PLUGIN_NAME} PROPERTIES
  OUTPUT_NAME "${PLUGIN_NAME}"
  BUNDLE true
  BUNDLE_EXTENSION "driver"
  PREFIX ""
  SUFFIX ""
  MACOSX_BUNDLE ON
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
  MACOSX_BUNDLE_BUNDLE_NAME "${PLUGIN_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
  MACOSX_BUNDLE_COPYRIGHT "${PLUGIN_COPYRIGHT}"
  MACOSX_BUNDLE_GUI_IDENTIFIER "${PLUGIN_BUNDLE}"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
    COMMENT "Stripping ${PLUGIN_NAME}"
    COMMAND
      strip -x $<TARGET_FILE:${PLUGIN_NAME}>
  )
endif()

if(NOT CODESIGN_ID STREQUAL "")
  add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
    COMMENT "Signing ${PLUGIN_NAME}.driver with ID ${CODESIGN_ID}"
    VERBATIM
    COMMAND
      codesign --force -s ${CODESIGN_ID}
        ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.driver
      )
endif()

add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
  COMMENT "Copying ${PLUGIN_NAME} to bin"
  COMMAND mkdir -p
    ${PROJECT_ROOT}/bin
  COMMAND rm -rf
    ${PROJECT_ROOT}/bin/${PLUGIN_NAME}.driver
  COMMAND cp -a
    ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_NAME}.driver
    ${PROJECT_ROOT}/bin
)

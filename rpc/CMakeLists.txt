set(RPC_FILES
  driver_protocol
  )

foreach(FILE IN LISTS RPC_FILES)
  add_custom_command(
    COMMENT "Running protoc on ${FILE}.proto"
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.proto
    OUTPUT
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.pb.h
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.pb.cc
    COMMAND
      ${GRPC_BIN_DIR}/protoc
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.proto
      )

  add_custom_command(
    COMMENT "Running grpc_cpp_plugin on ${FILE}.proto"
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.proto
    OUTPUT
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.grpc.pb.h
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.grpc.pb.cc
    COMMAND
      ${GRPC_BIN_DIR}/protoc
        --plugin=protoc-gen-grpc=${GRPC_BIN_DIR}/grpc_cpp_plugin
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.proto
      )

  list(APPEND GENERATED_SOURCES
    ${FILE}.pb.cc
    ${FILE}.grpc.pb.cc
  )
endforeach()

add_library(${RPC_LIB} STATIC
  ${GENERATED_SOURCES}
)

set_target_properties(${RPC_LIB} PROPERTIES
  # disable warnings for generated code
  COMPILE_FLAGS "-w"
)

configure_file(
  "build_info.cpp.in"
  "build_info.cpp"
)

add_library(${COMMON_LIB} STATIC
  "${CMAKE_CURRENT_BINARY_DIR}/build_info.cpp"
  "plist_info.mm"
)

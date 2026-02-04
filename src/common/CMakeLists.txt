add_library(common STATIC
  log.cpp
  net.cpp
  protocol.cpp
)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

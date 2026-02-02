add_executable(client main.cpp)
target_link_libraries(client PRIVATE common)
target_include_directories(client PRIVATE ../common)

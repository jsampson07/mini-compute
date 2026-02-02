add_executable(coordinator main.cpp)
target_link_libraries(coordinator PRIVATE common)
target_include_directories(coordinator PRIVATE ../common)

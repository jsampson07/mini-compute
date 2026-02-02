add_executable(worker main.cpp)
target_link_libraries(worker PRIVATE common)
target_include_directories(worker PRIVATE ../common)

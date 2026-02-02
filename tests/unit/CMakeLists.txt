# Executes test_stanity.cpp
add_executable(unit_tests test_sanity.cpp)
# Links a common library ("unit_tests")
target_link_libraries(unit_tests PRIVATE common)
target_include_directories(unit_tests PRIVATE ../../src/common)

add_test(NAME unit_tests COMMAND unit_tests)

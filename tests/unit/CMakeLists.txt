# Executes test_sanity.cpp
add_executable(unit_tests test_sanity.cpp)
add_executable(test_net test_net.cpp)
# Links a common library ("unit_tests")
target_link_libraries(unit_tests PRIVATE common)
target_include_directories(unit_tests PRIVATE ../../src/common)

target_link_libraries(test_net PRIVATE common)
target_include_directories(test_net PRIVATE ../../src/common)

add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME test_net COMMAND test_net)

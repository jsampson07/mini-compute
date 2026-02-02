cmake_minimum_required(VERSION 3.16)
# sets the language
project(mini_compute LANGUAGES CXX)

# sets the standard of environment
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# nice warnings
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing() # enables testing (turns it on)

# pulls in subdirectories
add_subdirectory(src)
add_subdirectory(tests)

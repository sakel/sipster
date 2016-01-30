cmake_minimum_required (VERSION 3.4.1)
project (sipster)

enable_testing()

#-Wpedantic -std=gnu99
#set(CMAKE_C_FLAGS "-Wall -Werror -g3 -O0")
set(CMAKE_CXX_FLAGS "-Wall -Werror -g3 -O0")

add_subdirectory (src)
add_subdirectory (test)

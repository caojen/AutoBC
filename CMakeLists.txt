cmake_minimum_required(VERSION 3.0)

project (autobc)

set(CMAKE_CXX_STANDARD 11)

AUX_SOURCE_DIRECTORY(src/ltl SRC_LIST)
AUX_SOURCE_DIRECTORY(src SRC_LIST)

include_directories(include/ltl)

add_compile_options(-Wall -g)

add_executable(autobc ${SRC_LIST})

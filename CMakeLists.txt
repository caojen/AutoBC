cmake_minimum_required(VERSION 3.0)

project (rander)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(LTL_BASIC src/dict.cpp src/literal.cpp src/ltl.cpp src/operator.cpp src/sat.cpp)

add_compile_options(-Wall -g)

add_executable(rander ${LTL_BASIC} src/rnd.cpp src/rander.cpp)

project(ltlin2post)

add_executable(ltlin2post ${LTL_BASIC} src/ltlin2post.cpp)

project(test)
add_executable(test ${LTL_BASIC} src/test.cpp)

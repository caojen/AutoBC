cmake_minimum_required(VERSION 3.0)

project (rander)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(include/ltl)
include_directories(include/rander)
include_directories(include/autobc)

set(LTL_BASIC src/ltl/dict.cpp src/ltl/literal.cpp src/ltl/ltl.cpp src/ltl/operator.cpp src/ltl/sat.cpp)

add_compile_options(-Wall -g)

add_executable(rander ${LTL_BASIC} src/rander/rnd.cpp src/rander/rander.cpp)

project(ltlin2post)

add_executable(ltlin2post ${LTL_BASIC} src/ltlin2post/ltlin2post.cpp)

project(mytest)
add_executable(mytest ${LTL_BASIC} src/test.cpp)

project(autobc)

add_executable(autobc ${LTL_BASIC} src/autobc/autobc.cpp src/autobc/main.cpp)

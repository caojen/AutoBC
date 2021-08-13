cmake_minimum_required(VERSION 3.0)

project (rander)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(include/ltl)
include_directories(include/rander)
include_directories(include/autobc)
include_directories(include/param)

set(LTL_BASIC src/ltl/dict.cpp src/ltl/literal.cpp src/ltl/ltl.cpp src/ltl/operator.cpp src/ltl/sat.cpp src/ltl/model_counter.cpp src/param/param.cpp)

add_compile_options(-Wall -g)

add_executable(rander ${LTL_BASIC} src/rander/rnd.cpp src/rander/rander.cpp)
target_link_libraries(rander -lgmp -lgmpxx)

project(ltlin2post)

add_executable(ltlin2post ${LTL_BASIC} src/ltlin2post/ltlin2post.cpp)
target_link_libraries(ltlin2post -lgmp -lgmpxx)

project(mytest)
add_executable(mytest ${LTL_BASIC} src/test.cpp)
target_link_libraries(mytest -lgmp -lgmpxx)

project(autobc)

add_executable(autobc ${LTL_BASIC} src/autobc/autobc.cpp src/autobc/fix_solver.cpp src/autobc/lasso.cpp src/autobc/main.cpp src/autobc/ranking.cpp)
target_link_libraries(autobc -lgmp -lgmpxx)

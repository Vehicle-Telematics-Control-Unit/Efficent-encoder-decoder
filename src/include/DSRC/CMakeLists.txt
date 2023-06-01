set(BUILD_LIB_NAME dsrc)
cmake_minimum_required(VERSION 3.22)
include_directories(${Boost_INCLUDE_DIR} ../)
add_library(${BUILD_LIB_NAME} SHARED ${CMAKE_CURRENT_SOURCE_DIR}/DSRC.cpp)
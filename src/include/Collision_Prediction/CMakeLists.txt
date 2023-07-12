set(BUILD_LIB_NAME Collision_Prediction)
cmake_minimum_required(VERSION 3.22)
add_library(${BUILD_LIB_NAME} SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Collision_Prediction.cpp)
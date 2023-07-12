cmake_minimum_required(VERSION 3.22)
set(FILES encoder.cpp)
set(include_dirs include include/DSRC include/unity include/someipService include/nlohmann)

set(CMAKE_BUILD_TYPE Debug)

#set(Boost_INCLUDE_DIR "/media/jok3r/0c8aed27-573e-4ed6-acb9-bd13e9be4906/boost_1_81_0")
#set(CMAKE_C_COMPILER "/home/jok3r/x-tools/aarch64-rpi4-linux-gnu/bin/aarch64-rpi4-linux-gnu-gcc")
#set(CMAKE_CXX_COMPILER "/home/jok3r/x-tools/aarch64-rpi4-linux-gnu/bin/aarch64-rpi4-linux-gnu-g++")
#set(CMAKE_ASM_COMPILER "/home/jok3r/x-tools/aarch64-rpi4-linux-gnu/bin/aarch64-rpi4-linux-gnu-gcc")
#set(CMAKE_OBJCOPY "/home/jok3r/x-tools/aarch64-rpi4-linux-gnu/bin/aarch64-rpi4-linux-gnu-objcopy")

project(main)
add_subdirectory(include/DSRC)
add_subdirectory(include/unity)
add_executable(${PROJECT_NAME} ${FILES})

if(NOT EXISTS "RPI")
message("TARGET IS SET TO UNITY!")
set(target "UNITY")
target_link_libraries(${PROJECT_NAME} dsrc unity )

else()

message("TARGET IS SET TO RPI")
set(target "RPI")
add_definitions(-DRPI)
################
### VSOMEIP ####
################
find_package (vsomeip3 REQUIRED)
find_package(Boost 1.58 COMPONENTS system thread log REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${VSOMEIP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} dsrc vsomeip3 Collision_Prediction ${Boost_LIBRARIES})
################
### /VSOMEIP ###
################
endif()

message(STATUS "target is now set to ${target}")

target_include_directories(${PROJECT_NAME} PRIVATE ${include_dirs})



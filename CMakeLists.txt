cmake_minimum_required(VERSION 2.8)
project(Heimdall)

# add new module for FindZeroMQ to work with find_package
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# set all executables to be put in main build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

find_package(ZeroMQ REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Qt5Widgets)

# if on MAC then use clang++ with libc++
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# if on linux then use g++ with c++11
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories(${CMAKE_SOURCE_DIR} ${ZEROMQ_INCLUDE_DIR})

add_subdirectory(Backbone)
add_subdirectory(Display)
add_subdirectory(EmergentSearch)
add_subdirectory(GUIRec)
add_subdirectory(GUISaliency)
add_subdirectory(ImagePush)
add_subdirectory(Orthorect)
add_subdirectory(Publish)
add_subdirectory(Recognition)
add_subdirectory(Saliency)
add_subdirectory(SharedUtils)
add_subdirectory(Verification)

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/debug)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/bdstore)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/psin)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/psstore)

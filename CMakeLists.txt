cmake_minimum_required(VERSION 2.8)
project(NUAUVSI)

# add new module for FindZeroMQ to work with find_package
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(ZeroMQ REQUIRED)
find_package(OpenCV)
find_package(Qt4)

# have to set to g++ or will default to clang
set(CMAKE_CXX_COMPILER "g++")

# if on MAC then use libc++
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# if on linux then use c++11
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories(${CMAKE_SOURCE_DIR} ${ZEROMQ_INCLUDE_DIR})

# generate binaries in build instead of build/Backbone
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(Backbone)
add_subdirectory(Display)
add_subdirectory(GUIRec)
add_subdirectory(GUISaliency)
add_subdirectory(ImagePub)
add_subdirectory(ImagePush)
add_subdirectory(OGRSaliency)
add_subdirectory(Publish)
add_subdirectory(Recognition)
add_subdirectory(Verification)


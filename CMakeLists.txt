cmake_minimum_required(VERSION 2.8)
project(NUAUVSI)

# have to set to g++ or will default to clang
set(CMAKE_CXX_COMPILER "g++")

# set compiler flags
set(CMAKE_CXX_FLAGS "-g")

# if on MAC then use libc++
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# if on linux then use c++11
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(CMAKE_EXE_LINKER_FLAGS "-lzmq -lpthread")

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Snippet saved for when linking libraries
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#
#find_package(ZeroMQ REQUIRED)
#include_directories(${ZEROMQ_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${ZEROMQ_LIBRARIES})

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


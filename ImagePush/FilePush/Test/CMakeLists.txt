# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testfilepush_source_files
    TestFilePush.cpp
	)

add_executable(testFilePush ${testfilepush_source_files})

target_link_libraries(testFilePush filepush)

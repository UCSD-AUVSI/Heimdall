# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testfolderpush_source_files
    TestFolderPush.cpp
	)

add_executable(testFolderPush ${testfolderpush_source_files})

target_link_libraries(testFolderPush filepush)

# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testfolderwatch_source_files
    TestFolderWatch.cpp
	)

add_executable(testFolderWatch ${testfolderwatch_source_files})

target_link_libraries(testFolderWatch folderwatch)

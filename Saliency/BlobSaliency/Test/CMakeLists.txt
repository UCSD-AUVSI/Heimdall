# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testblobsaliency_source_files
    TestBlobSaliency.cpp
	)

add_executable(testBlobSaliency ${testblobsaliency_source_files})

target_link_libraries(testBlobSaliency blobsaliency)

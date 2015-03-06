# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testssaliency_source_files
    TestSSaliency.cpp
	)

add_executable(testSSaliency ${testssaliency_source_files})

target_link_libraries(testSSaliency ssaliency)

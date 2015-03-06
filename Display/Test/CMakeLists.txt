# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testbdsub_source_files
	TestBDSub.cpp
	)

add_executable(testBDSub ${testbdsub_source_files})

target_link_libraries(testBDSub bdsub)


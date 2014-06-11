# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(test_orthorect_source_files
	TestOrthorect.cpp
	)

add_executable(test_orthorect ${test_orthorect_source_files})

target_link_libraries(test_orthorect orthorect)

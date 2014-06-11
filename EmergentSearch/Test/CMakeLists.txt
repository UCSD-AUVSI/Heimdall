# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testpssub_source_files
	TestPSSub.cpp
	)

add_executable(testPSSub ${testpssub_source_files})

target_link_libraries(testPSSub pssub)


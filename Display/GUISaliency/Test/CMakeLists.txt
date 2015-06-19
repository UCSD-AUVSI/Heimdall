# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testguisaliency_source_files
	TestGUISaliency
	)

add_executable(testGUISaliency ${testguisaliency_source_files})

target_link_libraries(testGUISaliency guisaliency)

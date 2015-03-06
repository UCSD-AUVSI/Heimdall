# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testpublish_source_files
	TestPublish.cpp
	)

add_executable(testPublish ${testpublish_source_files})

target_link_libraries(testPublish publish)

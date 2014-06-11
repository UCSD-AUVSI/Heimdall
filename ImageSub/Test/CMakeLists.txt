# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testimagesub_source_files
	TestImageSub.cpp
	)

add_executable(testImageSub ${testimagesub_source_files})

target_link_libraries(testImageSub imagesub)

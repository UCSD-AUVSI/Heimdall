# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testguirec_source_files
	TestGUIRec.cpp
	)

add_executable(testGUIRec ${testguirec_source_files})

target_link_libraries(testGUIRec guirec)

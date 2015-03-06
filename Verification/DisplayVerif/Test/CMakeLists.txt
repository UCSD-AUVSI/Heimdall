# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(test_displayverif_source_files
    test_display_verif.cpp
	)

add_executable(test_displayverif ${test_displayverif_source_files})

target_link_libraries(test_displayverif displayverif)

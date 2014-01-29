set(testdisplay_source_files
	TestDisplay.cpp
	)

add_executable(testDisplay ${testdisplay_source_files})

target_link_libraries(testDisplay display)


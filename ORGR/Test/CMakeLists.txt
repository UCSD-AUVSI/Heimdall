set(testorgr_source_files
	TestORGR.cpp
	)

add_executable(testORGR ${testorgr_source_files})

target_link_libraries(testORGR orgr)

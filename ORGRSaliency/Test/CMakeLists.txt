set(testorgr_source_files
	TestORGR.cpp
	)
set(testsaliency_source_files
	TestSaliency.cpp
	)

add_executable(testORGR ${testorgr_source_files})
add_executable(testSaliency ${testsaliency_source_files})

target_link_libraries(testORGR orgr)
target_link_libraries(testSaliency saliency)


set(testsaliency_source_files
	TestSaliency.cpp
	)

add_executable(testSaliency ${testsaliency_source_files})

target_link_libraries(testSaliency saliency)


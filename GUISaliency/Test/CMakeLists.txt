set(testguisaliency_source_files
	TestGUISaliency
	)

add_executable(testGUISaliency ${testguisaliency_source_files})

target_link_libraries(testGUISaliency guisaliency)

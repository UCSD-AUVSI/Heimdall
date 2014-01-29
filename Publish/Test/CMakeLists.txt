set(testpublish_source_files
	TestPublish.cpp
	)

add_executable(testPublish ${testpublish_source_files})

target_link_libraries(testPublish publish)


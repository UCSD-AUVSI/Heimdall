set(testimagepush_source_files
	TestImagePush.cpp
	)

add_executable(testImagePush ${testimagepush_source_files})

target_link_libraries(testImagePush imagepush)

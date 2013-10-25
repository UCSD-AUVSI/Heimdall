set(testimagepub_source_files
	TestImagePub.cpp
	)

add_executable(testImagePub ${testimagepub_source_files})

target_link_libraries(testImagePub imagepub)

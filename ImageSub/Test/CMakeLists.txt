set(testimagesub_source_files
	TestImageSub.cpp
	)

add_executable(testImageSub ${testimagesub_source_files})

target_link_libraries(testImageSub imagesub)

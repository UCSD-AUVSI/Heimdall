set(testfilepush_source_files
    TestFilePush.cpp
	)

add_executable(testFilePush ${testfilepush_source_files})

target_link_libraries(testFilePush imagepush)

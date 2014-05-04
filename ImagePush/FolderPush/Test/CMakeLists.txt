set(testfolderpush_source_files
    TestFolderPush.cpp
	)

add_executable(testFolderPush ${testfolderpush_source_files})

target_link_libraries(testFolderPush filepush)

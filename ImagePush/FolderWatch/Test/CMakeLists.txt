set(testfolderwatch_source_files
    TestFolderWatch.cpp
	)

add_executable(testFolderWatch ${testfolderwatch_source_files})

target_link_libraries(testFolderWatch folderwatch)

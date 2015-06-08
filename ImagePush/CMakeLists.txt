set(imagepush_source_files
	stub_push.hpp
	stub_push.cpp
	)

add_subdirectory(FilePush)
add_subdirectory(FolderPush)
add_subdirectory(FolderWatch2014)
add_subdirectory(FolderWatch2015)

add_library(imagepush ${imagepush_source_files})

target_link_libraries(imagepush filepush folderpush folderwatch2014 folderwatch2015 sharedutils backbone ${OpenCV_LIBS})

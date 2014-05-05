set(imagepush_source_files
	stub_push.hpp
	stub_push.cpp
	)

add_subdirectory(FilePush)
add_subdirectory(FolderPush)

add_library(imagepush ${imagepush_source_files})

target_link_libraries(imagepush filepush folderpush backbone ${OpenCV_LIBS})

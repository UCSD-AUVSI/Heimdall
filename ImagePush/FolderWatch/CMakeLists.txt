set(folderwatch_source_files
	folder_watch.hpp
	folder_watch.cpp
	)

add_library(folderwatch ${folderwatch_source_files})

target_link_libraries(folderwatch imagepush ${OpenCV_LIBS})

add_subdirectory(Test)

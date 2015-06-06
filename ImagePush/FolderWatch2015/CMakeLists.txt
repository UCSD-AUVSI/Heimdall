set(folderwatch2015_source_files
	folder_watch_2015.hpp
	folder_watch_2015.cpp
	)

add_library(folderwatch2015 ${folderwatch2015_source_files})

target_link_libraries(folderwatch2015 imagepush ${OpenCV_LIBS})


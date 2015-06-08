set(folderwatch2014_source_files
	folder_watch_2014.hpp
	folder_watch_2014.cpp
	)

add_library(folderwatch2014 ${folderwatch2014_source_files})

target_link_libraries(folderwatch2014 imagepush ${OpenCV_LIBS})


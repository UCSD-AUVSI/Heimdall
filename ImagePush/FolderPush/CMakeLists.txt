set(folderpush_source_files
	folder_push.hpp
	folder_push.cpp
	)

add_library(folderpush ${folderpush_source_files})

target_link_libraries(folderpush imagepush ${OpenCV_LIBS})

add_subdirectory(Test)

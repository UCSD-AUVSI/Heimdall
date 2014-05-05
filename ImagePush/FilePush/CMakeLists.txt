set(filepush_source_files
	file_push.hpp
	file_push.cpp
	)

add_library(filepush ${filepush_source_files})

target_link_libraries(filepush imagepush ${OpenCV_LIBS})

add_subdirectory(Test)

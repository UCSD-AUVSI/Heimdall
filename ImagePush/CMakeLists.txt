set(imagepush_source_files
	FilePush.hpp
	FilePush.cpp
	)

add_library(imagepush ${imagepush_source_files})

target_link_libraries(imagepush backbone)

add_subdirectory(Test)

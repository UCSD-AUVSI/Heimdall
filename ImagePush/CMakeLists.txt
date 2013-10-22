set(imagepush_source_lists
	ImagePush.hpp
	ImagePush.cpp
	)

add_library(imagepush ${imagepush_source_lists})

target_link_libraries(imagepush backbone)

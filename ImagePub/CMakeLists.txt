set(imagepub_source_files
	ImagePub.hpp
	ImagePub.cpp
	)

add_library(imagepub ${imagepub_source_files})

target_link_libraries(imagepub backbone)

set(publish_source_files
	Publish.hpp
	Publish.cpp
	)

add_library(publish ${publish_source_files})

target_link_libraries(publish backbone)

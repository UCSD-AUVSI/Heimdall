set(saliency_source_files
	Saliency.hpp
	Saliency.cpp
	)

add_library(saliency ${saliency_source_files})

target_link_libraries(saliency backbone)

add_subdirectory(Test)

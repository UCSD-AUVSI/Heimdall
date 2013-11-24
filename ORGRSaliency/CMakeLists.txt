set(orgr_source_files
	ORGR.hpp
	ORGR.cpp
	)
set(saliency_source_files
	Saliency.hpp
	Saliency.cpp
	)

add_library(orgr ${orgr_source_files})
add_library(saliency ${saliency_source_files})

target_link_libraries(orgr backbone)
target_link_libraries(saliency backbone)

add_subdirectory(Test)

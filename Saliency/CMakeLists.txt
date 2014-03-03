set(saliency_source_files
	Saliency.hpp
	Saliency.cpp
	saliency_module_C_SBD.cpp
	saliency_module_C_SBD.hpp
	saliency_module_template.hpp
	SSaliency.hpp
	SSaliency.cpp
	)

add_library(saliency ${saliency_source_files})

target_link_libraries(saliency backbone)

add_subdirectory(Test)

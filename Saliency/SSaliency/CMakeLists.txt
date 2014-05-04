set(ssaliency_source_files
    saliency_module_C_SBD.cpp
	saliency_module_C_SBD.hpp
	saliency_module_template.hpp
	ssaliency.hpp
	ssaliency.cpp
	)

add_library(ssaliency ${ssaliency_source_files})

target_link_libraries(ssaliency saliency)

add_subdirectory(Test)

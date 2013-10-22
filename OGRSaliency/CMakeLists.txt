set(orthorect_source_files
	Orthorect.hpp
	Orthorect.cpp
	)
set(georef_source_files
	GeoRef.hpp
	GeoRef.cpp
	)
set(saliency_source_files
	Saliency.hpp
	Saliency.cpp
	)

add_library(orthorect ${orthorect_source_files})
add_library(georef ${georef_source_files})
add_library(saliency ${saliency_source_files})

target_link_libraries(orthorect backbone)
target_link_libraries(georef backbone)
target_link_libraries(saliency backbone)

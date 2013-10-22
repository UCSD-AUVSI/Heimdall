set(ogrsaliency_source_files
	Orthorect.hpp
	Orthorect.cpp
	GeoRef.hpp
	GeoRef.cpp
	Saliency.hpp
	Saliency.cpp
	)

add_library(ogrsaliency ${ogrsaliency_source_files})

target_link_libraries(ogrsaliency backbone)

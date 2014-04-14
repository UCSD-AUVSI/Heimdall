set(srec_source_files
	TemplateSRec.cpp
	TemplateSRec.hpp
	)

add_subdirectory(PolygonShapeRec)

add_library(shaperec ${srec_source_files})

target_link_libraries(shaperec polygon_shaperec backbone)


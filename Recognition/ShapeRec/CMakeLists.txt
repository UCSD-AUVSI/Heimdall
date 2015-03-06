set(srec_source_files
	stub_srec.cpp
	stub_srec.hpp
	)

add_subdirectory(PolygonShapeRec)

add_library(shaperec ${srec_source_files})

target_link_libraries(shaperec polygon_shaperec backbone ${OpenCV_LIBS})


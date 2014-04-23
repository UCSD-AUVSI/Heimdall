set(gocr_ocr_source_files
	GOCR_BackboneInterface.cpp
	GOCR_BackboneInterface.hpp
	ocr_algorithm_gocr.cpp
	ocr_algorithm_gocr.hpp
	)

add_library(gocr_ocr ${gocr_ocr_source_files})

target_link_libraries(gocr_ocr ocr_utils sharedutils ocr)


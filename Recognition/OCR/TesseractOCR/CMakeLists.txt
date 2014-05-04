set(tess_ocr_source_files
	ocr_algorithm_tesseract.cpp
	ocr_algorithm_tesseract.hpp
	TessOCR_BackboneInterface.cpp
	TessOCR_BackboneInterface.hpp
	)

add_library(tess_ocr ${tess_ocr_source_files})

target_link_libraries(tess_ocr tesseract lept ocr_utils sharedutils ocr)


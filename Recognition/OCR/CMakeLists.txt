set(ocr_source_files
	TessOCR.hpp
	TessOCR.cpp
	)

add_library(ocr ${ocr_source_files})

target_link_libraries(ocr backbone)

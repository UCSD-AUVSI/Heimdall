set(ocr_source_files
	StubOCR.hpp
	StubOCR.cpp
	)

add_library(ocr ${ocr_source_files})

target_link_libraries(ocr backbone)

set(ocr_source_files
	StubOCR.hpp
	StubOCR.cpp
	)

add_subdirectory(TesseractOCR)

add_library(ocr ${ocr_source_files})

target_link_libraries(ocr tess_ocr backbone)

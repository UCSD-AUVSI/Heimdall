set(ocr_source_files
	StubOCR.hpp
	StubOCR.cpp
	)

add_subdirectory(TesseractOCR)
add_subdirectory(GOCR)
add_subdirectory(OCRUtils)

add_library(ocr ${ocr_source_files})

target_link_libraries(ocr ocr_utils gocr_ocr tess_ocr backbone)

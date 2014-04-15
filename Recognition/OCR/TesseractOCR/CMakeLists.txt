set(tess_ocr_source_files
	ocr_algorithm_template.hpp
	ocr_algorithm_tesseract.cpp
	ocr_algorithm_tesseract.hpp
	ocr_module_main.cpp
	ocr_module_main.hpp
	ocr_results.cpp
	ocr_results.hpp
	TessOCR_BackboneInterface.cpp
	TessOCR_BackboneInterface.hpp
	test_data_results_ocr.cpp
	test_data_results_ocr.hpp
	)

add_library(tess_ocr ${tess_ocr_source_files})

target_link_libraries(tess_ocr tesseract lept sharedutils ocr)
#target_link_libraries(tess_ocr ocr)

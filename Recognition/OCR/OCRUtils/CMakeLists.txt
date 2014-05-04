set(ocr_utils_source_files
	ocr_algorithm_template.cpp
	ocr_algorithm_template.hpp
	ocr_module_main.cpp
	ocr_module_main.hpp
	ocr_results.cpp
	ocr_results.hpp
	test_data_results_ocr.cpp
	test_data_results_ocr.hpp
	)

add_library(ocr_utils ${ocr_utils_source_files})

target_link_libraries(ocr_utils sharedutils ocr)


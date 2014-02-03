set(testtessocr_source_files
	TestTessOCR.cpp
	)
set(testtemplatesrec_source_files
	TestTemplateSRec.cpp
	)

add_executable(testTemplateSRec ${testtemplatesrec_source_files})
add_executable(testTessOCR ${testtessocr_source_files})

target_link_libraries(testTemplateSRec shaperec ${OpenCV_LIBS})
target_link_libraries(testTessOCR ocr ${OpenCV_LIBS})

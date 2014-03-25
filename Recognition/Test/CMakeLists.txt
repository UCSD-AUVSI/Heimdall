set(teststubocr_source_files
	TestStubOCR.cpp
	)
set(testtemplatesrec_source_files
	TestTemplateSRec.cpp
	)

add_executable(testTemplateSRec ${testtemplatesrec_source_files})
add_executable(testStubOCR ${teststubocr_source_files})

target_link_libraries(testTemplateSRec shaperec ${OpenCV_LIBS})
target_link_libraries(testStubOCR ocr ${OpenCV_LIBS})

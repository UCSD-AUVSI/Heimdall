set(testtessocr_source_files
	TestTessOCR.cpp
	)
set(testtemplatesrec_source_files
	TestTemplateSRec.cpp
	)
set(testerosionseg_source_files
	TestErosionSeg.cpp
	)

add_executable(testErosionSeg ${testerosionseg_source_files})
add_executable(testTemplateSRec ${testtemplatesrec_source_files})
add_executable(testTessOCR ${testtessocr_source_files})

target_link_libraries(testTemplateSRec shaperec ${OpenCV_LIBS})
target_link_libraries(testTessOCR ocr ${OpenCV_LIBS})
target_link_libraries(testErosionSeg seg ${OpenCV_LIBS})

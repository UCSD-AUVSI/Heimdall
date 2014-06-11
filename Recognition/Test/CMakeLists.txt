SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)


# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(teststubocr_source_files
	TestStubOCR.cpp
	)
set(testtessocr_source_files
	TestTessOCR.cpp
	)
set(testtemplatesrec_source_files
	TestTemplateSRec.cpp
	)

add_executable(testTemplateSRec ${testtemplatesrec_source_files})
add_executable(testStubOCR ${teststubocr_source_files})
add_executable(testTessOCR ${testtessocr_source_files})

target_link_libraries(testTemplateSRec shaperec ${OpenCV_LIBS})
target_link_libraries(testStubOCR ocr ${OpenCV_LIBS})
target_link_libraries(testTessOCR sharedutils ocr_utils tess_ocr ocr ${OpenCV_LIBS})

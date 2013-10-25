set(testscseg_source_files
	TestSCSegmentation.cpp
	)
set(testscrecog_source_files
	TestSCRecognition.cpp
	)
set(testsegmentation_source_files
	TestSegmentation.cpp
	)

add_executable(testShapeCharSeg ${testscseg_source_files})
add_executable(testShapeCharRecog ${testscrecog_source_files})
add_executable(testSegmentation ${testsegmentation_source_files})

target_link_libraries(testShapeCharSeg charshapeseg)
target_link_libraries(testShapeCharRecog charshaperecog)
target_link_libraries(testSegmentation segmentation ${OpenCV_LIBS})

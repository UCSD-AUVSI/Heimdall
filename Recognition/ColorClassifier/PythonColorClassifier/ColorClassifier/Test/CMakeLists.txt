# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(TestColorClassifier_source_files
    TestColorClassifier.cpp
	)

add_executable(testPythonColorClassifier ${TestColorClassifier_source_files})

target_link_libraries(testPythonColorClassifier pythoncolorclass sharedutils ${ZEROMQ_LIBRARIES})




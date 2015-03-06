# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(TestExampleColorClassifier_source_files
    TestExampleColorClassifier.cpp
	)

add_executable(testPythonExampleColorClassifier ${TestExampleColorClassifier_source_files})

target_link_libraries(testPythonExampleColorClassifier pythoncolorclass sharedutils ${ZEROMQ_LIBRARIES})




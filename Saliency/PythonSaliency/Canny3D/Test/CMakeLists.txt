# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testspython3dcannysaliency_source_files
    TestPythonCanny3D.cpp
	)

add_executable(testPythonCanny3Dsaliency ${testspython3dcannysaliency_source_files})

target_link_libraries(testPythonCanny3Dsaliency pythonsaliency sharedutils ${ZEROMQ_LIBRARIES})






set(optimizepython3dcannysaliency_source_files
    GeneticOptimizeCanny3D.cpp
	)

add_executable(testGeneticOptimizeCanny3D ${optimizepython3dcannysaliency_source_files})

target_link_libraries(testGeneticOptimizeCanny3D pythonsaliency sharedutils SaliencyResultsTruthTesterLibrary EnableKeyPressToExit ${ZEROMQ_LIBRARIES})


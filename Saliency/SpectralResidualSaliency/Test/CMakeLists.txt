# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(testsspectralsaliency_source_files
    TestSpectralResidualSaliency.cpp
	)

add_executable(testSpectralResidualSaliency ${testsspectralsaliency_source_files})

target_link_libraries(testSpectralResidualSaliency spectralsaliency ${ZEROMQ_LIBRARIES})





# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(OptimizeSpectralResidualSaliency_source_files
    OptimizeSpectralResidualSaliency.cpp
	)

add_executable(testOptimizeSpectralResidualSaliency ${OptimizeSpectralResidualSaliency_source_files})

target_link_libraries(testOptimizeSpectralResidualSaliency spectralsaliency optimization EnableKeyPressToExit ${ZEROMQ_LIBRARIES})




set(saliencytruthresults_library_source_files
	SaliencyResultsTruthTester.cpp
	SaliencyResultsTruthTester.hpp
	SaliencyExperimentResults.hpp
	SaliencyExperimentResults.cpp
	optimizeSaliency.hpp
	optimizeSaliency.cpp
	)

add_library(SaliencyResultsTruthTesterLibrary ${saliencytruthresults_library_source_files})

target_link_libraries(SaliencyResultsTruthTesterLibrary sharedutils truthreader EnableKeyPressToExit ${OpenCV_LIBS})



# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(saliencytruthresults_executable_source_files
	tester_main.cpp
	)

add_executable(SaliencyResultsTruthTester ${saliencytruthresults_executable_source_files})

target_link_libraries(SaliencyResultsTruthTester SaliencyResultsTruthTesterLibrary ${OpenCV_LIBS})

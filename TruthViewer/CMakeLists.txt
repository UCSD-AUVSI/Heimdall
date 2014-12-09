
# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(truth_viewer_ReaderLib_files
	TruthFile.cpp
	TruthFile.hpp
	)

set(truth_viewer_executable_files
	TruthViewer.cpp
	)

add_library(truthreader ${truth_viewer_ReaderLib_files})

add_executable(TruthViewer ${truth_viewer_executable_files})

target_link_libraries(TruthViewer truthreader sharedutils ${OpenCV_LIBS})

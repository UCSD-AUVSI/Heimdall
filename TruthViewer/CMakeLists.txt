
# set all executables to be put in test build directory
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(truth_viewer_source_files
	TruthViewer.cpp
	TruthFile.cpp
	TruthFile.hpp
	)

add_executable(TruthViewer ${truth_viewer_source_files})

target_link_libraries(TruthViewer sharedutils ${OpenCV_LIBS})

set(pythonsegmentation_source_files
	PythonSegmentation_ModuleInterface.cpp
	PythonSegmentation_ModuleInterface.hpp
	PythonSegmentation.cpp
	PythonSegmentation.hpp
)

add_library(python_seg ${pythonsegmentation_source_files})

target_link_libraries(python_seg sharedutilspython seg ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


# put the shared library where the python files will be able to use it
SET(LIBRARY_OUTPUT_PATH ../Python)

set(pykmeansppcpplib_source_files
	KmeansPP_BPythonInterface.cpp
	)

SET(CMAKE_SHARED_MODULE_PREFIX )

add_library(pykmeansppcpplib MODULE ${pykmeansppcpplib_source_files})

target_link_libraries(pykmeansppcpplib sharedutilspython sharedutilspython_cvmatconv clustering_generic sharedutils ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


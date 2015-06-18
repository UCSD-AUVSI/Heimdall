
# put the shared library where the python files will be able to use it
SET(LIBRARY_OUTPUT_PATH ../../../../Results/Python)

set(pydbscancpplib_source_files
	DBSCAN_BPythonInterface.cpp
	)

SET(CMAKE_SHARED_MODULE_PREFIX )

add_library(pydbscancpplib MODULE ${pydbscancpplib_source_files})

target_link_libraries(pydbscancpplib sharedutilspython sharedutilspython_cvmatconv clustering_generic ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


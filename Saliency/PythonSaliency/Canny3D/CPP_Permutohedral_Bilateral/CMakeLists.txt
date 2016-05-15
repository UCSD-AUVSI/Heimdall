
# put the shared library where the python files will be able to use it
SET(LIBRARY_OUTPUT_PATH ../Python)

set(pypermutohedralfiltercpplib_source_files
	Permutohedral_BPythonInterface.cpp
	PHImage.cpp
	PHImage.h
	permutohedral.h
	)

SET(CMAKE_SHARED_MODULE_PREFIX )
add_library(pypermutohedralfiltercpplib MODULE ${pypermutohedralfiltercpplib_source_files})
target_link_libraries(pypermutohedralfiltercpplib sharedutilspython sharedutilspython_cvmatconv ${OpenCV_LIBS})
include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


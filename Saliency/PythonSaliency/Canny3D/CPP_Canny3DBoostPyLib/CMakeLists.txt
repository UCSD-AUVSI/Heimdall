
# put the shared library where the python files will be able to use it
SET(LIBRARY_OUTPUT_PATH ../Python)

set(pycanny3dcpplib_source_files
	Canny3D_BPythonInterface.cpp
	canny_modified_from_v2_4_9.cpp
	canny_modified_from_v2_4_9.hpp
	precomp_v2_4_9.hpp
	)

SET(CMAKE_SHARED_MODULE_PREFIX )

add_library(pycanny3dcpplib MODULE ${pycanny3dcpplib_source_files})

target_link_libraries(pycanny3dcpplib sharedutilspython ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


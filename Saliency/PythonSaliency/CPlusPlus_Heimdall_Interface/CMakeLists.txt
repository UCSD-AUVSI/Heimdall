set(pythonsaliency_source_files
    PythonSaliency_ModuleInterface.cpp
    PythonSaliency_ModuleInterface.hpp
	PythonSaliency.cpp
	PythonSaliency.hpp
)

add_library(pythonsaliency ${pythonsaliency_source_files})

target_link_libraries(pythonsaliency sharedutilspython saliency ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

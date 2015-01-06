set(pythoncolorclassifier_source_files
    PythonColorClassifier_ModuleInterface.cpp
    PythonColorClassifier_ModuleInterface.hpp
	PythonColorClassifier.cpp
	PythonColorClassifier.hpp
)

add_library(pythoncolorclass ${pythoncolorclassifier_source_files})

target_link_libraries(pythoncolorclass sharedutilspython colorclass ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

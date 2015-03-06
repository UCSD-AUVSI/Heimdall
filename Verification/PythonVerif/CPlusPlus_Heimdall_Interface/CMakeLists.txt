set(pythonverif_source_files
    PythonVerif_ModuleInterface.cpp
    PythonVerif_ModuleInterface.hpp
	PythonVerif.cpp
	PythonVerif.hpp
)

add_library(pythonverif ${pythonverif_source_files})

target_link_libraries(pythonverif sharedutilspython verification ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

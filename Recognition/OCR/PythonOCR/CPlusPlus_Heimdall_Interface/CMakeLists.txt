set(pythonocr_source_files
    PythonOCR_ModuleInterface.cpp
    PythonOCR_ModuleInterface.hpp
    PythonOCR.cpp
    PythonOCR.hpp
)

add_library(pythonocr ${pythonocr_source_files})

target_link_libraries(pythonocr sharedutilspython ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

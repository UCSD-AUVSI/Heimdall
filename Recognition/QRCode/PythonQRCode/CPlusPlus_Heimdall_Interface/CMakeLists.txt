set(pythonqrcode_source_files
	PythonQRCode_ModuleInterface.cpp
	PythonQRCode_ModuleInterface.hpp
	PythonQRCode.cpp
	PythonQRCode.hpp
)

add_library(pythonqrcode ${pythonqrcode_source_files})

target_link_libraries(pythonqrcode sharedutilspython qrcodeprocessor ${OpenCV_LIBS})

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

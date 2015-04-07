set(qrcode_source_files
	stub_qrcode.cpp
	stub_qrcode.hpp
	)

add_subdirectory(PythonQRCode)

add_library(qrcodeprocessor ${qrcode_source_files})

target_link_libraries(qrcodeprocessor pythonqrcode backbone ${OpenCV_LIBS})


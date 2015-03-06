set(orthorect_source_files
	stub_orthorect.hpp
	stub_orthorect.cpp
	)

add_library(orthorect ${orthorect_source_files})

target_link_libraries(orthorect backbone ${OpenCV_LIBS})

add_subdirectory(Test)

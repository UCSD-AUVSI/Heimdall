set(orthorect_source_files
	Orthorect.hpp
	Orthorect.cpp
	)

add_library(orthorect ${orthorect_source_files})

target_link_libraries(orthorect backbone)

add_subdirectory(Test)

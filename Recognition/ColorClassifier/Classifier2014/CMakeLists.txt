set(colorclass_2014_source_files
	colorclassifier2014.cpp
	colorclassifier2014.hpp
	)

add_library(colorclass_2014 ${colorclass_2014_source_files})

target_link_libraries(colorclass_2014 colorclass ${OpenCV_LIBS})

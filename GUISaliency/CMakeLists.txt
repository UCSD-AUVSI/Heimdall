set(guisaliency_source_files
	GUISaliency.hpp
	GUISaliency.cpp
	)

add_library(guisaliency ${guisaliency_source_files})

target_link_libraries(guisaliency backbone ${OpenCV_LIBS})

add_subdirectory(Test)

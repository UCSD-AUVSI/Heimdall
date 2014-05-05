set(imagesub_source_files
	ImageSub.hpp
	ImageSub.cpp
	)

add_library(imagesub ${imagesub_source_files})

target_link_libraries(imagesub backbone ${OpenCV_LIBS})

add_subdirectory(Test)

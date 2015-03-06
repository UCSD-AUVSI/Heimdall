set(displayverif_source_files
    display_verif.hpp
	display_verif.cpp
	)

add_library(displayverif ${displayverif_source_files})

target_link_libraries(displayverif verification ${OpenCV_LIBS})

add_subdirectory(Test)

set(display_source_files 
	Display.cpp
	Display.hpp
	)

add_library(display ${display_source_files})

target_link_libraries(display backbone)

add_subdirectory(Test)


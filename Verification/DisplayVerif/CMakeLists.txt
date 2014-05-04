set(display_verif_source_files
    display_verif.hpp
	display_verif.cpp
	)

add_library(display_verif ${display_verif_source_files})

target_link_libraries(display_verif verification)

add_subdirectory(Test)

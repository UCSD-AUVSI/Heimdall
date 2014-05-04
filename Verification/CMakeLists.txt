set(verification_source_files
	stub_verif.hpp
	stub_verif.cpp
	)

add_subdirectory(DisplayVerif)

add_library(verification ${verification_source_files})

target_link_libraries(verification display_verif backbone)

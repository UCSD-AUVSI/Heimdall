set(verification_source_files
	Verif.hpp
	Verif.cpp
	)

add_library(verification ${verification_source_files})

target_link_libraries(verification backbone)

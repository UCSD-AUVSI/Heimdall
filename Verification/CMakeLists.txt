set(verification_source_files
	stub_verif.hpp
	stub_verif.cpp
	)

add_subdirectory(DisplayVerif)
add_subdirectory(PlaneVerif)
add_subdirectory(ExperimentVerif)

add_library(verification ${verification_source_files})

target_link_libraries(verification displayverif planeverif experimentverif backbone ${OpenCV_LIBS})

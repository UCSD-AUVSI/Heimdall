set(experimentverif_source_files
    experiment_verif.hpp
	experiment_verif.cpp
	)

add_library(experimentverif ${experimentverif_source_files})

target_link_libraries(experimentverif verification ${OpenCV_LIBS})


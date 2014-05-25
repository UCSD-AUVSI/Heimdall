set(planeverif_source_files
    plane_verif.hpp
	plane_verif.cpp
	)

add_library(planeverif ${planeverif_source_files})

target_link_libraries(planeverif verification ${OpenCV_LIBS})

add_subdirectory(Test)

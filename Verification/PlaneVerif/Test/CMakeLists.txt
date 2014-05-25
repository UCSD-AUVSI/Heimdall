set(test_planeverif_source_files
    test_plane_verif.cpp
	)

add_executable(test_planeverif ${test_planeverif_source_files})

target_link_libraries(test_planeverif planeverif)

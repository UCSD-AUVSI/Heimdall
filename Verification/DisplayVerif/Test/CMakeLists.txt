set(test_display_verif_source_files
    test_display_verif.cpp
	)

add_executable(test_display_verif ${test_display_verif_source_files})

target_link_libraries(test_display_verif display_verif)


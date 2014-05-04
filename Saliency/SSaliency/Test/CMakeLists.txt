set(testssaliency_source_files
    TestSSaliency.cpp
	)

add_executable(testSSaliency ${testssaliency_source_files})

target_link_libraries(testSSaliency ssaliency)

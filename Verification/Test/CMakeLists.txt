set(testverif_source_files
	TestVerification.cpp
	)

add_executable(testVerification ${testverif_source_files})

target_link_libraries(testVerification verification)


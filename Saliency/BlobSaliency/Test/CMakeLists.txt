set(testblobsaliency_source_files
    TestBlobSaliency.cpp
	)

add_executable(testBlobSaliency ${testblobsaliency_source_files})

target_link_libraries(testBlobSaliency blobsaliency)

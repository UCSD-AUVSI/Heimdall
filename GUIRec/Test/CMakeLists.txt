set(testguirec_source_files
	TestGUIRec.cpp
	)

add_executable(testGUIRec ${testguirec_source_files})

target_link_libraries(testGUIRec guirec)

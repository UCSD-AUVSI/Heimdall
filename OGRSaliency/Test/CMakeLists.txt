set(testgeoref_source_files
	TestGeoref.cpp
	)
set(testorthorect_source_files
	TestOrthorect.cpp
	)
set(testsaliency_source_files
	TestSaliency.cpp
	)

add_executable(testGeoRef ${testgeoref_source_files})
add_executable(testOrthorect ${testorthorect_source_files})
add_executable(testSaliency ${testsaliency_source_files})

target_link_libraries(testGeoRef georef)
target_link_libraries(testOrthorect orthorect)
target_link_libraries(testSaliency saliency)


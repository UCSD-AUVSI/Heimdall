set(orgr_source_files
	ORGR.hpp
	ORGR.cpp
	)

add_library(orgr ${orgr_source_files})

target_link_libraries(orgr backbone)

add_subdirectory(Test)

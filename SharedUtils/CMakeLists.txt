set(sharedutils_source_files
	SharedUtils.hpp
	SharedUtils.cpp
	SharedUtils_OpenCV.hpp
	SharedUtils_OpenCV.cpp
	)

add_library(sharedutils ${sharedutils_source_files})

target_link_libraries(sharedutils backbone)


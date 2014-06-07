set(sharedutils_source_files
	cvplot.cpp
	cvplot.h
	exif.cpp
	exif.h
    intersection.cpp
    intersection.hpp
	SharedUtils.hpp
	SharedUtils.cpp
	SharedUtils_OpenCV.hpp
	SharedUtils_OpenCV.cpp
	OS_FolderBrowser_tinydir.h
	)

add_subdirectory(gocr)

add_library(sharedutils ${sharedutils_source_files})

target_link_libraries(sharedutils gocr backbone)

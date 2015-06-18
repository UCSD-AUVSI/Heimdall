set(sharedutils_source_files
	clustering_opencv.cpp
	clustering_opencv.hpp
	cvplot.cpp
	cvplot.h
	exif.cpp
	exif.h
	Georeference.hpp
	Georeference.cpp
	intersection.cpp
	intersection.hpp
	SharedUtils.hpp
	SharedUtils.cpp
	SharedUtils_OpenCV.hpp
	SharedUtils_OpenCV.cpp
	OS_FolderBrowser_tinydir.h
	GlobalVars.hpp
	)

set(sharedutilspython_cvmatconvert_source_files
	PythonCVMatConvert.cpp
	PythonCVMatConvert.h
	)

set(sharedutilspython_source_files
	PythonUtils.hpp
	PythonUtils.cpp
	)

set(EnableKeyPressToExit_source_files
	EnableKeyPressToExit.cpp
	EnableKeyPressToExit.hpp
	)

add_subdirectory(optimization)
add_subdirectory(gocr)
add_subdirectory(clustering)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

add_library(EnableKeyPressToExit ${EnableKeyPressToExit_source_files})
add_library(sharedutils ${sharedutils_source_files})
add_library(sharedutilspython SHARED ${sharedutilspython_source_files})
add_library(sharedutilspython_cvmatconv SHARED ${sharedutilspython_cvmatconvert_source_files})

target_link_libraries(sharedutils gocr optimization backbone)
target_link_libraries(sharedutilspython python2.7 boost_python-py27)

include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})


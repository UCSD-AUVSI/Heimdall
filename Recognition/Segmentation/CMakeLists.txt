set(seg_source_files
	stub_seg.hpp
	stub_seg.cpp
	)

add_subdirectory(SkynetSegmentation)
add_subdirectory(ClusterSegmentation)
add_subdirectory(PythonSegmentation)

add_library(seg ${seg_source_files})

target_link_libraries(seg cluster_seg skynet_seg python_seg backbone ${OpenCV_LIBS})

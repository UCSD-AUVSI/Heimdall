set(cluster_seg_source_files
	ClusterSeg_BackboneInterface.cpp
	ClusterSeg_BackboneInterface.hpp
	ClusterSegMain.hpp
	ClusterSegMain.cpp
	)

add_library(cluster_seg ${cluster_seg_source_files})

target_link_libraries(cluster_seg seg ${OpenCV_LIBS})

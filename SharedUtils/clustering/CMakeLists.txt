set(clustering_generic_source_files
	clustering.cpp
	clustering.hpp
	DBSCAN.cpp
	DBSCAN.hpp
	kmeans.cpp
	kmeans.hpp
	kmeansplusplus.cpp
	kmeansplusplus.hpp
	)

add_library(clustering_generic ${clustering_generic_source_files})

target_link_libraries(clustering_generic cluster_seg ${OpenCV_LIBS})

set(seg_source_files
	ErosionSeg.hpp
	ErosionSeg.cpp
	BlobSeg.hpp
	BlobSeg.cpp
	)

add_library(seg ${seg_source_files})

target_link_libraries(seg backbone)

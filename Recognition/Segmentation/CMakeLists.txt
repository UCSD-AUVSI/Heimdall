set(seg_source_files
	BlobSeg.hpp
	BlobSeg.cpp
	)

add_library(seg ${seg_source_files})

target_link_libraries(seg backbone)

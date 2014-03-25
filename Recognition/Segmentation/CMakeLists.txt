set(seg_source_files
	StubSeg.hpp
	StubSeg.cpp
	)

add_subdirectory(SkynetSegmentation)

add_library(seg ${seg_source_files})

target_link_libraries(seg skynet_seg backbone)

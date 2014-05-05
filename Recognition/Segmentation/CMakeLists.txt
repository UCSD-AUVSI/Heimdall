set(seg_source_files
	stub_seg.hpp
	stub_seg.cpp
	)

add_subdirectory(SkynetSegmentation)

add_library(seg ${seg_source_files})

target_link_libraries(seg skynet_seg backbone ${OpenCV_LIBS})

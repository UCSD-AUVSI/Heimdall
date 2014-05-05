set(skynet_seg_source_files
	Segmentation_CSEG_MultiReturn.cpp
	Segmentation_CSEG_MultiReturn.hpp
	Segmentation_CSEG_SSEG_Merger.cpp
	Segmentation_CSEG_SSEG_Merger.hpp
	SegmentationSettings.hpp
	Segmentation_SSEG_and_CSEG_and_Merger.cpp
	Segmentation_SSEG_and_CSEG_and_Merger.hpp
	Segmentation_SSEG_MultiReturn.cpp
	Segmentation_SSEG_MultiReturn.hpp
	SkynetSeg_BackboneInterface.cpp
	SkynetSeg_BackboneInterface.hpp
	skynet_BlobContour.cpp
	skynet_BlobContour.hpp
	skynet_blob.cpp
	skynet_blob.hpp
	skynet_BlobLibraryConfiguration.hpp
	skynet_BlobOperators.cpp
	skynet_BlobOperators.hpp
	skynet_BlobResult.cpp
	skynet_BlobResult.hpp
	skynet_ColorBlob.cpp
	skynet_ColorBlob.hpp
	skynet_ComponentLabeling.cpp
	skynet_ComponentLabeling.hpp
	skynet_HistogramSegmenter.cpp
	skynet_HistogramSegmenter.hpp
	skynet_Segmenter_SingleImageReturn.cpp
	skynet_Segmenter_SingleImageReturn.hpp
	skynet_SegmenterTemplate.hpp
	skynet_VisionUtil.cpp
	skynet_VisionUtil.hpp
	test_data_results_segmentation.cpp
	test_data_results_segmentation.hpp
	)

add_library(skynet_seg ${skynet_seg_source_files})

target_link_libraries(skynet_seg seg ${OpenCV_LIBS})

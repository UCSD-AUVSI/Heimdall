set(blobsaliency_source_files
    blob_saliency_module.cpp
	blob_saliency_module.hpp
	blob_saliency_module_template.hpp
	blob_saliency.hpp
	blob_saliency.cpp
	)

add_library(blobsaliency ${blobsaliency_source_files})

target_link_libraries(blobsaliency saliency ${OpenCV_LIBS})

add_subdirectory(Test)

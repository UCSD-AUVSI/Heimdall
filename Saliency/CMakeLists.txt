set(saliency_source_files
    stub_saliency.hpp
    stub_saliency.cpp
)

add_subdirectory(SSaliency)
add_subdirectory(BlobSaliency)

add_library(saliency ${saliency_source_files})

target_link_libraries(saliency ssaliency blobsaliency backbone ${OpenCV_LIBS})

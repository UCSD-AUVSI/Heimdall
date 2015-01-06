set(saliency_source_files
    stub_saliency.hpp
    stub_saliency.cpp
	GenericSaliencyImplementation.hpp
)

add_subdirectory(SSaliency)
add_subdirectory(BlobSaliency)
add_subdirectory(ResultsTruthTester)
add_subdirectory(PythonSaliency)
add_subdirectory(SpectralResidualSaliency)

add_library(saliency ${saliency_source_files})

target_link_libraries(saliency pythonsaliency spectralsaliency ssaliency blobsaliency backbone ${OpenCV_LIBS})


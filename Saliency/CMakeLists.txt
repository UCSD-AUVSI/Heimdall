set(saliency_source_files
	stub_saliency.hpp
	stub_saliency.cpp
)

set(saliency_opt_source_files
	OptimizeableSaliency.hpp
	OptimizeableSaliency.cpp
	CalculateSaliencyExperimentResults.cpp
)

add_subdirectory(SSaliency)
add_subdirectory(BlobSaliency)
add_subdirectory(ResultsTruthTester)
add_subdirectory(PythonSaliency)
add_subdirectory(SpectralResidualSaliency)

add_library(saliency ${saliency_source_files})
add_library(saliencyoptimization ${saliency_opt_source_files})

target_link_libraries(saliencyoptimization SaliencyResultsTruthTesterLibrary ${OpenCV_LIBS})

target_link_libraries(saliency saliencyoptimization pythonsaliency spectralsaliency ssaliency blobsaliency backbone ${OpenCV_LIBS})


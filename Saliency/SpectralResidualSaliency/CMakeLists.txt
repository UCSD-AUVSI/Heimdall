set(spectralsaliency_source_files
	spectral_residual_saliency_module.cpp
	spectral_residual_saliency_module.hpp
	modifiedSpectralResidualSaliency.cpp
	modifiedSpectralResidualSaliency.hpp
	ProcessingClass.hpp
	ProcessingClass.cpp
)

add_library(spectralsaliency ${spectralsaliency_source_files})

target_link_libraries(spectralsaliency saliency ${OpenCV_LIBS})

add_subdirectory(Test)

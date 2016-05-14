
# put the shared library where the python files will be able to use it
# SET(LIBRARY_OUTPUT_PATH ..)

set(bpSRsaliencylib_source_files
	BPythonInterface.cpp
	modifiedSpectralResidualSaliency.cpp
	modifiedSpectralResidualSaliency.hpp
	)

SET(CMAKE_SHARED_MODULE_PREFIX )
add_library(bpSRsaliencylib MODULE ${bpSRsaliencylib_source_files})
target_link_libraries(bpSRsaliencylib sharedutilspython sharedutilspython_cvmatconv ${OpenCV_LIBS})
include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

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

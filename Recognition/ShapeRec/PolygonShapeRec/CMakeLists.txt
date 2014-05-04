set(polygon_shaperec_source_files
	Shaperec_ModuleMain.cpp
	Shaperec_ModuleMain.hpp
	Shaperec_SingleImgAlg_template.hpp
	Shaperec_SingleImgAlg_Turning.cpp
	Shaperec_SingleImgAlg_Turning.hpp
	Shaperec_Utils_Results.cpp
	Shaperec_Utils_Results.hpp
	Shaperec_Utils_TurningAlgorithm.cpp
	Shaperec_Utils_TurningAlgorithm.hpp
	Shaperec_BackboneInterface.cpp
	Shaperec_BackboneInterface.hpp
	test_data_results_shaperec.cpp
	test_data_results_shaperec.hpp
	)

add_library(polygon_shaperec ${polygon_shaperec_source_files})

file(COPY shaperec_reference_shapes DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(polygon_shaperec shaperec)

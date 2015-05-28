set(polygon_shaperec_source_files
	Shaperec_BackboneInterface.cpp
	Shaperec_BackboneInterface.hpp
	)
set(polygon_shaperec_lib_source_files
	Shaperec_ModuleMain.cpp
	Shaperec_ModuleMain.hpp
	Shaperec_SingleImgAlg_template.hpp
	Shaperec_SingleImgAlg_Turning.cpp
	Shaperec_SingleImgAlg_Turning.hpp
	Shaperec_Utils_Results.cpp
	Shaperec_Utils_Results.hpp
	Shaperec_Utils_TurningAlgorithm.cpp
	Shaperec_Utils_TurningAlgorithm.hpp
	test_data_results_shaperec.cpp
	test_data_results_shaperec.hpp
	)

set(pypolyshapereccpplib_source_files
	BPythonInterface.cpp
	)

add_library(pypolyshaperec SHARED ${pypolyshapereccpplib_source_files})
include_directories(${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_PATH})

add_library(polygon_shaperec ${polygon_shaperec_source_files})
add_library(polygon_shaperec_lib SHARED ${polygon_shaperec_lib_source_files})

file(COPY shaperec_reference_shapes DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY test_shaperec.py DESTINATION ${CMAKE_BINARY_DIR}/Recognition/Segmentation/PythonSegmentation/MonalSegmentation/Python)
file(COPY test_shaperec.py DESTINATION ${CMAKE_BINARY_DIR}/Recognition/ShapeRec/PolygonShapeRec) 
file(COPY ${CMAKE_BINARY_DIR}/Recognition/ShapeRec/PolygonShapeRec/libpypolyshaperec.so DESTINATION ${CMAKE_BINARY_DIR}/Recognition/Segmentation/PythonSegmentation/MonalSegmentation/Python)




target_link_libraries(polygon_shaperec_lib ${OpenCV_LIBS})
target_link_libraries(polygon_shaperec shaperec polygon_shaperec_lib ${OpenCV_LIBS})
target_link_libraries(pypolyshaperec polygon_shaperec_lib sharedutilspython sharedutilspython_cvmatconv sharedutils ${OpenCV_LIBS})


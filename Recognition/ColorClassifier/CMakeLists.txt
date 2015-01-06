set(colorclassifier_source_files
	stub_colorclassifier.cpp
	stub_colorclassifier.hpp
	)

add_subdirectory(Classifier2014)
add_subdirectory(PythonColorClassifier)

add_library(colorclass ${colorclassifier_source_files})

target_link_libraries(colorclass colorclass_2014 pythoncolorclass backbone ${OpenCV_LIBS})


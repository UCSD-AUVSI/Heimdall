set(recognition_source_files
	CharacterRecognition.hpp
	CharacterRecognition.cpp
	CharacterSegmentation.hpp
	CharacterSegmentation.cpp
	Segmentation.hpp
	Segmentation.cpp
	ShapeRecognition.hpp
	ShapeRecognition.cpp
	ShapeSegmentation.hpp
	ShapeSegmentation.cpp
	)

add_library(recognition ${recognition_source_files})

target_link_libraries(recognition backbone)

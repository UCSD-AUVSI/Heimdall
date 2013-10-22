set(segmentation_source_files
	Segmentation.hpp
	Segmentation.cpp
	)
set(csrecog_source_files
	CharacterRecognition.hpp
	CharacterRecognition.cpp
	ShapeRecognition.hpp
	ShapeRecognition.cpp
	)
set(csseg_source_files
	CharacterSegmentation.hpp
	CharacterSegmentation.cpp
	ShapeSegmentation.hpp
	ShapeSegmentation.cpp
	)

add_library(segmentation ${segmentation_source_files})
add_library(charshaperecog ${csrecog_source_files})
add_library(charshapeseg ${csseg_source_files})

target_link_libraries(segmentation backbone)
target_link_libraries(charshaperecog backbone)
target_link_libraries(charshapeseg backbone)

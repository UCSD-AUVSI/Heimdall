set(guirec_source_files
	GUIRec.hpp
	GUIRec.cpp
	)

add_library(guirec ${guirec_source_files})

target_link_libraries(guirec backbone ${OpenCV_LIBS})

add_subdirectory(Test)

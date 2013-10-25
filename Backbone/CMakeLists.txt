# generate binaries in build instead of build/Backbone
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(backbone_source_files
	Algorithm.hpp
	Backbone.hpp
	BackStore.hpp
	BackStore.cpp
	IMGData.cpp
	IMGData.hpp
	)
set(dist_source_files
	Algs.hpp
	Maps.hpp
	Maps.cpp
	DistWorker.cpp
	)
set(push_source_files
	PushWorker.cpp
	)
set(server_source_files
	Server.cpp
	)

add_library(backbone ${backbone_source_files})
add_library(dist ${dist_source_files})
add_library(push ${push_source_files})
add_library(server ${server_source_files})

add_executable(zServer ${server_source_files})
add_executable(zPushWorker ${push_source_files})
add_executable(zDistWorker ${dist_source_files})

target_link_libraries(
	zDistWorker
	dist
	orthorect
	georef
	saliency
	segmentation
	charshapeseg
	charshaperecog
	display
	guirec
	guisaliency
	imagepub
	imagepush
	publish
	verification
	${ZEROMQ_LIBRARIES}
	)
target_link_libraries(
	zServer
	server
	backbone
	${ZEROMQ_LIBRARIES}
	)
target_link_libraries(zPushWorker 
	${ZEROMQ_LIBRARIES}
	)

if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})
	target_link_libraries(zDistWorker ${OpenCV_LIBS})
endif(OpenCV_FOUND)
if(Qt_FOUND)
	include_directories(${QT_USE_FILE})
	target_link_libraries(zDistWorker ${QT_LIBRARIES})
endif(Qt_FOUND)


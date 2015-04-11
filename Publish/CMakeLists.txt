set(publish_source_files
	Publish.hpp
	Publish.cpp
	)

add_library(publish ${publish_source_files})

add_executable(Publish ${publish_source_files})

target_link_libraries(
    Publish 
    publish
    sharedutils
    backbone
    ${PTHREAD_LIB}
    ${ZEROMQ_LIBRARIES}
    ${OpenCV_LIBS}
    )

add_subdirectory(Test)

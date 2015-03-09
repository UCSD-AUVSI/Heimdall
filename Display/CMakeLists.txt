set(bdsub_source_files 
    BDSub.cpp
	BDSub.hpp
	)

add_library(bdsub ${bdsub_source_files})

add_executable(BDSub ${bdsub_source_files})

target_link_libraries(
    BDSub 
    backbone 
    sharedutils
    ${PTHREAD_LIB}
    ${ZEROMQ_LIBRARIES}
    ${OpenCV_LIBS}
    )

add_subdirectory(BigDog)
add_subdirectory(Test)

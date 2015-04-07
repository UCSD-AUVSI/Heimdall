set(pssub_source_files 
    PSSub.cpp
	PSSub.hpp
	)

add_library(pssub ${pssub_source_files})

add_executable(PSSub ${pssub_source_files})

target_link_libraries(
    PSSub 
    backbone 
    ${PTHREAD_LIB}
    ${ZEROMQ_LIBRARIES}
    ${OpenCV_LIBS}
    )

add_subdirectory(PopUpStalker)
add_subdirectory(Test)

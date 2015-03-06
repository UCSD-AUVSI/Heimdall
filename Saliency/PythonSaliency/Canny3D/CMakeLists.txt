
file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Saliency/PythonSaliency/Canny3D)

add_subdirectory(CPP_Canny3DBoostPyLib)
add_subdirectory(Test)


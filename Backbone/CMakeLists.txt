# generate binaries in build instead of build/Backbone
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(backbone_source_files
    AUVSI_Algorithm.hpp
    Backbone.hpp
    Maps.hpp
    Maps.cpp
    BackStore.cpp
    BackStore.hpp
    IMGData.cpp
    IMGData.hpp
    MessageHandling.cpp
    MessageHandling.hpp
    PortHandling.cpp
    PortHandling.hpp
    Client.hpp
    )
set(dist_source_files
    DistAlgs.hpp
    DistClient.cpp
    DistClient.hpp
    DistMaps.hpp
    DistMaps.cpp
    )
set(server_source_files
    Server.cpp
    )

add_library(backbone ${backbone_source_files})
add_library(dist ${dist_source_files})
add_library(server ${server_source_files})

add_executable(zServer ${server_source_files})
add_executable(zDistClient ${dist_source_files})

target_link_libraries(
    zDistClient
    dist
    backbone
    orthorect
    saliency
    sharedutils
    seg
    ocr
    shaperec
    display
    guirec
    guisaliency
    imagepush
    imagesub
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

if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(zDistClient ${OpenCV_LIBS})
    target_link_libraries(zServer ${OpenCV_LIBS})
endif(OpenCV_FOUND)
if(Qt_FOUND)
    include_directories(${QT_USE_FILE})
    target_link_libraries(zDistClient ${QT_LIBRARIES})
endif(Qt_FOUND)


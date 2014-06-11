set(popupstalker_source_files 
    main.cpp
    maintab.cpp
    mainwindow.cpp
    model.cpp
    viewcontroller.cpp
    util.cpp
    )       

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Tell CMake to create the BigDog executable
add_executable(PopUpStalker ${popupstalker_source_files})

# Use the Widgets module from Qt 5.
target_link_libraries(PopUpStalker Qt5::Widgets)

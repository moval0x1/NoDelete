cmake_minimum_required(VERSION 3.16)

project(NoDelete VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Include headers directory
include_directories(${CMAKE_SOURCE_DIR}/src/headers)

# Include the manifest file
set(MANIFEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/app.manifest")

# Organize files into specific folders
set(HEADERS
    src/headers/mainwindow.h
    src/headers/windowsManagement.h
    src/headers/util.h
)

set(SOURCES
    src/sources/main.cpp
    src/sources/mainwindow.cpp
    src/sources/windowsManagement.cpp
    src/sources/util.cpp
)

set(UIS
    src/ui/mainwindow.ui
    src/ui/res.qrc
    src/ui/app_icon.rc
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/src/ui
)

# Add executable
add_executable(NoDelete
    ${HEADERS}
    ${SOURCES}
    ${UIS}
    ${MANIFEST_FILE}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NoDelete)
endif()

# Link Qt libraries
target_link_libraries(NoDelete PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Set application properties
set_target_properties(NoDelete PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.NoDelete"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    LINK_FLAGS "/MANIFESTUAC:NO"  # Disable automatic manifest to avoid conflicts
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS NoDelete
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

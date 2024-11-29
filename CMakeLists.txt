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

# Organize files into specific folders
set(HEADERS
    src/headers/mainwindow.h
    src/headers/WindowsManagement.h
)

set(SOURCES
    src/sources/main.cpp
    src/sources/mainwindow.cpp
    src/sources/WindowsManagement.cpp
)

set(UIS
    src/ui/mainwindow.ui
    src/ui/res.qrc
    src/ui/app_icon.rc
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_SOURCE_DIR}/src/ui
)

qt_add_executable(NoDelete
    MANUAL_FINALIZATION
    ${HEADERS}
    ${SOURCES}
    ${UIS}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NoDelete)
endif()

target_link_libraries(NoDelete PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.NoDelete)
endif()
set_target_properties(NoDelete PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS NoDelete
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

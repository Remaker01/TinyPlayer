cmake_minimum_required(VERSION 3.5)

project(TinyPlayer VERSION 3.10 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VLCQt_DIR vlc-qt_1.1.0/lib/cmake/VLCQt)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-mwindows -O2 -s -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-mwindows -Os -s -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O -g")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "-O1 -DNDEBUG")
endif()
set(INCLUDE_DIRECTORIES
    ./qm
    ./images
    ./src)
set(TS_FILES TinyPlayer_zh_CN.ts)

set(PROJECT_SOURCES
        src/global_include.h
        src/playerwindow.cpp src/playerwindow.h src/playerwindow.ui
        src/player.cpp src/player.h
	src/playerbutton.cpp src/playerbutton.h
        src/playerslider.cpp src/playerslider.h
        src/playlistview.h src/playlistview.cpp
        src/main.cpp
        src/music.h src/music.cpp src/formatCheck.cpp
        src/settingwindow.h src/settingwindow.cpp src/settingwindow.ui
        src/onlineseacher.h src/onlineseacher.cpp
        src/searchresultwidget.h src/searchresultwidget.cpp src/searchresultwidget.ui
        src/framelesswindow.h src/framelesswindow.cpp
        resource.qrc
        ${TS_FILES}
)
set(HELP_FILES  readme.htm)
include_directories(vlc-qt_1.1.0/include)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network LinguistTools REQUIRED)
find_package(VLCQt COMPONENTS Core REQUIRED)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TinyPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${HELP_FILES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TinyPlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(TinyPlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
# for Windows
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        add_executable(TinyPlayer
            ${PROJECT_SOURCES}
            ${HELP_FILES}
            meta.rc
        )
    else()
        add_executable(TinyPlayer
            ${PROJECT_SOURCES}
            ${HELP_FILES}
        )
    endif()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(TinyPlayer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    VLCQt::Core
)

set_target_properties(TinyPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TinyPlayer)
endif()

cmake_minimum_required(VERSION 3.5)

project(TinyPlayer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools multimedia REQUIRED)
set(CMAKE_CXX_FLAGS_RELEASE "-mwindows -O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-mwindows -Os -s -DNDEBUG")
set(INCLUDE_DIRECTORIES
    ./qm
    ./images
    ./src)
set(TS_FILES TinyPlayer_zh_CN.ts)

set(PROJECT_SOURCES
        src/playerwindow.cpp
        src/playerwindow.h
        src/playerwindow.ui
        src/player.cpp
        src/player.h
        src/playerbutton.cpp
        src/playerbutton.h
        src/playerslider.cpp
        src/playerslider.h
        src/playlistview.h
        src/playlistview.cpp
        src/main.cpp
        src/music.h
        src/music.cpp
        resource.qrc
        #qm.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TinyPlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
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
            meta.rc
        )
    else()
        add_executable(TinyPlayer
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(TinyPlayer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    )

set_target_properties(TinyPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TinyPlayer)
endif()

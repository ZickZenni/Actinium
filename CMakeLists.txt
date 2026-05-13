cmake_minimum_required(VERSION 4.3)
project(Actinium)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Actinium src/main.cpp
        src/core/application.cpp
        src/core/application.h
        src/core/main_window.cpp
        src/core/main_window.h)

target_link_libraries(Actinium
        Qt::Core
        Qt::Gui
        Qt::Widgets
)


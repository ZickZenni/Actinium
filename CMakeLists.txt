cmake_minimum_required(VERSION 4.2.2)
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

add_subdirectory(thirdparty/uuid)

add_executable(Actinium src/main.cpp
        src/core/application.cpp
        src/core/application.h
        src/ui/window/main_window.cpp
        src/ui/window/main_window.h
        src/instance/instance.cpp
        src/instance/instance.h
        src/game/game.cpp
        src/game/game.h)

target_link_libraries(Actinium
        Qt::Core
        Qt::Gui
        Qt::Widgets
        uuid_v4::uuid_v4
)

target_include_directories(Actinium PRIVATE src/)

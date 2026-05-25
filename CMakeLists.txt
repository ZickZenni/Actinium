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
add_subdirectory(thirdparty/json)

add_executable(Actinium src/main.cpp
        src/core/application.cpp
        src/core/application.h
        src/ui/window/main_window.cpp
        src/ui/window/main_window.h
        src/instance/instance.cpp
        src/instance/instance.h
        src/game/game.h
        src/ui/dialog/create_instance_dialog.cpp
        src/ui/dialog/create_instance_dialog.h
        src/util/json.h)

target_link_libraries(Actinium
        Qt::Core
        Qt::Gui
        Qt::Widgets
        uuid_v4::uuid_v4
        nlohmann_json
)

target_include_directories(Actinium PRIVATE src/)

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

add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/spdlog)

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
        src/util/json.h
        src/util/path.h
        src/util/path.cpp
        src/ui/model/instance_model.cpp
        src/ui/model/instance_model.h
        src/ui/view/instance_view.cpp
        src/ui/view/instance_view.h
        src/ui/delegate/instance_delegate.cpp
        src/ui/delegate/instance_delegate.h
        src/util/qt.h
        src/util/qt.cpp)

add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(Actinium copy_resources)

target_link_libraries(Actinium
        Qt::Core
        Qt::Gui
        Qt::Widgets
        nlohmann_json
        spdlog::spdlog
)

target_include_directories(Actinium PRIVATE src/)

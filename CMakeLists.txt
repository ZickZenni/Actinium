cmake_minimum_required(VERSION 3.24)
project(Actinium LANGUAGES CXX)

### CMake Options ###
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

### Compiler Flags ###
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### Version ###
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 1)

### Git ###
set(GIT_COMMIT_MESSAGE "unknown")
set(GIT_COMMIT_HASH "unknown")
set(GIT_BRANCH "unknown")

### Build ###
string(TIMESTAMP CURRENT_TIME "%d-%m-%Y %H:%M:%S")
set(BUILD_TIMESTAMP "${CURRENT_TIME}")

find_package(Git QUIET)

if (Git_FOUND)
    execute_process(
            COMMAND "${GIT_EXECUTABLE}" log -1 --format=%B
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_VARIABLE GIT_COMMIT_MESSAGE
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    execute_process(
            COMMAND "${GIT_EXECUTABLE}" log -1 --format=%h
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    execute_process(
            COMMAND "${GIT_EXECUTABLE}" branch --show-current
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    string(REPLACE "\\" "\\\\" GIT_COMMIT_MESSAGE "${GIT_COMMIT_MESSAGE}")
    string(REPLACE "\"" "\\\"" GIT_COMMIT_MESSAGE "${GIT_COMMIT_MESSAGE}")
    string(REPLACE "\n" " " GIT_COMMIT_MESSAGE "${GIT_COMMIT_MESSAGE}")
endif ()

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
)

add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/cpr)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/core/build_config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/build/build_config.h")

add_executable(Actinium
        src/ui/dialog/launch_instance_dialog.cpp
        src/ui/dialog/launch_instance_dialog.h
        src/worker/worker.h
        src/util/github.cpp
        src/util/github.h
        src/worker/task.h
        src/worker/tasks/download_loader_task.cpp
        src/worker/tasks/download_loader_task.h)

target_sources(Actinium PRIVATE
        src/main.cpp
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
        src/util/qt.cpp
        src/ui/widget/sidebar.cpp
        src/ui/widget/sidebar.h
        src/ui/window/instance_window.cpp
        src/ui/window/instance_window.h
        ${CMAKE_CURRENT_BINARY_DIR}/build_config.h
)

target_compile_definitions(Actinium PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

target_link_libraries(Actinium
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        nlohmann_json
        spdlog::spdlog
        cpr::cpr
)

target_include_directories(Actinium PRIVATE
        src/
        "${CMAKE_CURRENT_BINARY_DIR}/build/"
)

add_custom_target(TARGET Actinium POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/resources"
        "${CMAKE_CURRENT_BINARY_DIR}/resources"
)
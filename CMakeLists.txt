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

### Build ###
string(TIMESTAMP CURRENT_TIME "%d-%m-%Y %H:%M:%S")
set(BUILD_TIMESTAMP "${CURRENT_TIME}")

### CPR Options ###
set(BUILD_LIBCURL_DOCS OFF)
set(ENABLE_CURL_MANUAL OFF)

### Libraries ###
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(LibArchive REQUIRED)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/cpr)
add_subdirectory(thirdparty/semver)
add_subdirectory(thirdparty/simpleini)
add_subdirectory(thirdparty/efsw)

### Generate Build Config / Info file ###
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/core/build_config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/build/build_config.h")

### Executable ###
add_executable(Actinium
        src/core/download_manager.cpp
        src/core/download_manager.h)

### Source ###
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
        src/util/lib/json.h
        src/util/fs/path.h
        src/util/fs/path.cpp
        src/ui/model/instance_model.cpp
        src/ui/model/instance_model.h
        src/ui/view/instance_view.cpp
        src/ui/view/instance_view.h
        src/ui/delegate/instance_delegate.cpp
        src/ui/delegate/instance_delegate.h
        src/util/lib/qt.h
        src/util/lib/qt.cpp
        src/ui/widget/sidebar.cpp
        src/ui/widget/sidebar.h
        src/ui/window/instance_window.cpp
        src/ui/window/instance_window.h
        src/ui/dialog/launch_instance_dialog.cpp
        src/ui/dialog/launch_instance_dialog.h
        src/worker/worker.h
        src/util/api/github.cpp
        src/util/api/github.h
        src/task/task.h
        src/task/download_loader_task.cpp
        src/task/download_loader_task.h
        src/util/lib/archive.cpp
        src/util/lib/archive.h
        src/worker/worker.cpp
        src/util/fs/temp_file.h
        src/task/download_task.cpp
        src/task/download_task.h
        src/task/download_libraries_task.cpp
        src/task/download_libraries_task.h
        src/loader/migoto.cpp
        src/loader/migoto.h
        src/util/fs/symlink.cpp
        src/util/fs/symlink.h
        src/util/std/string.cpp
        src/util/std/string.h
        src/loader/loader_library.cpp
        src/loader/loader_library.h
        src/util/software/steam.cpp
        src/util/software/steam.h
        src/core/logger.h
        src/ui/widget/page_container.cpp
        src/ui/widget/page_container.h
        src/ui/page/page.h
        src/ui/page/instance/instance_mods_page.cpp
        src/ui/page/instance/instance_mods_page.h
        src/ui/page/instance/instance_settings_page.cpp
        src/ui/page/instance/instance_settings_page.h
        src/instance/installed_mod.h
        src/ui/view/mod_list_view.cpp
        src/ui/view/mod_list_view.h
        src/ui/model/proxy_model.h
        src/ui/window/download_mods_window.cpp
        src/ui/window/download_mods_window.h
        src/ui/window/base_window.h
        src/ui/window/base_window.cpp
        src/provider/provider.h
        src/provider/game_banana_provider.cpp
        src/provider/game_banana_provider.h
        src/ui/model/provider/provider_mod_model.cpp
        src/ui/model/provider/provider_mod_model.h
        src/ui/delegate/provider/provider_mod_delegate.cpp
        src/ui/delegate/provider/provider_mod_delegate.h
        src/ui/image_cache.cpp
        src/ui/image_cache.h
)

if (WIN32)
    target_sources(Actinium PRIVATE
            src/util/platform/win.cpp
            src/util/platform/win.h
    )
endif ()

### Defines ###
target_compile_definitions(Actinium PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

### Link Libraries ###
target_link_libraries(Actinium
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        nlohmann_json
        spdlog::spdlog
        cpr::cpr
        LibArchive::LibArchive
        semver
        SimpleIni::SimpleIni
        efsw
)

### Inclusions ###
target_include_directories(Actinium PRIVATE
        src/
        "${CMAKE_CURRENT_BINARY_DIR}/build/"
)

### Copy Resources to binary directory ###
add_custom_target(TARGET Actinium POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/resources"
        "${CMAKE_CURRENT_BINARY_DIR}/resources"
)
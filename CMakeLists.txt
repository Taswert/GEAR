cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GEAR VERSION 1.0.0)

add_library(${PROJECT_NAME} SHARED 
    src/main.cpp 
    src/ErGui.cpp
    src/modules/FooterModule.cpp
    src/modules/TransformObjectModule.cpp
    src/modules/EditGroupModule.cpp
    src/modules/SelectFilterModule.cpp
    src/includes/myUtils.cpp
    src/modules/ToolsModule.cpp
    src/modules/CameraSettings.cpp
    src/modules/ObjectListModule.cpp
    src/includes/ObjectCategories.cpp
    src/modules/LayerModule.cpp
    src/modules/EditObjectModule.cpp
    src/modules/ActionHistoryModule.cpp
    src/modules/EditColorModule.cpp
    src/modules/GameWindowModule.cpp
    src/modules/SettingsModule.cpp
    src/modules/EditorUtilsModule.cpp
    src/includes/razoomUtils.cpp
    src/modules/GlobalDockingView.cpp
    src/modules/ContextMenuModule.cpp
    src/features/AutoSave.cpp
    src/features/SelectedObjectInfo.cpp 
    src/features/ZoomToScroll.cpp 
    src/features/FixBPMTriggerCrash.cpp 
    src/features/MoreKeybinds.cpp
    src/features/CustomGridResize.cpp 
    src/features/ObjectListConfig.cpp 
    src/features/Hovering.cpp
    src/includes/CustomImGuiWidgets.cpp)


#include(FetchContent)
#FetchContent_Declare(
#    json
#    GIT_REPOSITORY https://github.com/nlohmann/json.git
#)
#FetchContent_MakeAvailable(json)


if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

set(HAS_IMGUI ON)
CPMAddPackage("gh:matcool/gd-imgui-cocos#0207a36bbe504d8dbc34c984218114af6af36afc")

target_link_libraries(${PROJECT_NAME} imgui-cocos) #nlohmann_json::nlohmann_json

target_include_directories(imgui-cocos INTERFACE ./library/imgui-docking)

    target_sources(imgui-cocos INTERFACE
        ./library/imgui-docking/imgui.cpp
        ./library/imgui-docking/imgui_widgets.cpp
        ./library/imgui-docking/imgui_tables.cpp
        ./library/imgui-docking/imgui_draw.cpp
        ./library/imgui-docking/imgui_demo.cpp
        ./library/imgui-docking/misc/cpp/imgui_stdlib.cpp
    )


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/includes)




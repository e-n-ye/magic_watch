cmake_minimum_required(VERSION 3.16)

project(watch_arch_learning C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/out")

option(WATCH_ARCH_ENABLE_LVGL "Build LVGL-based learning stages" ON)

if(WATCH_ARCH_ENABLE_LVGL)
    get_filename_component(MAGIC_WATCH_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    set(MAGIC_WATCH_SIM_ROOT "${MAGIC_WATCH_ROOT}/sim/lv_port_pc_vscode")

    find_package(SDL2 REQUIRED)

    set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
    set(
        LV_BUILD_CONF_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h"
        CACHE PATH
        "Watch architecture lab LVGL configuration"
        FORCE
    )

    add_subdirectory(
        "${MAGIC_WATCH_SIM_ROOT}/lvgl"
        "${CMAKE_BINARY_DIR}/lvgl"
    )

    target_include_directories(lvgl PUBLIC
        "${MAGIC_WATCH_SIM_ROOT}"
        ${SDL2_INCLUDE_DIRS}
    )

    add_subdirectory(runtime)
    add_subdirectory(stage_00_direct)
endif()

add_subdirectory(stage_01_console_direct)
add_subdirectory(stage_02_context_menu)
add_subdirectory(stage_03_multi_input_direct)

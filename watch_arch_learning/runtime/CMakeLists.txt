add_library(watch_arch_runtime STATIC
    lvgl_sdl_runtime.c
)

target_include_directories(watch_arch_runtime PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(watch_arch_runtime PUBLIC
    lvgl
    ${SDL2_LIBRARIES}
)

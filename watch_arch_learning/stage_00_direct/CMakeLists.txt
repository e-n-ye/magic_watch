add_executable(watch_arch_stage_00_direct
    main.c
    app_direct.c
)

target_include_directories(watch_arch_stage_00_direct PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(watch_arch_stage_00_direct PRIVATE
    watch_arch_runtime
)

if(NOT MSVC)
    target_link_libraries(watch_arch_stage_00_direct PRIVATE m pthread)
endif()

if(WIN32)
    if(MSVC)
        target_link_options(watch_arch_stage_00_direct PRIVATE "/SUBSYSTEM:CONSOLE")
    else()
        target_link_options(watch_arch_stage_00_direct PRIVATE "-mconsole")
    endif()
endif()

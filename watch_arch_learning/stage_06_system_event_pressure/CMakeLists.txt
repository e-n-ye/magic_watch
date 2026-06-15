add_executable(watch_arch_stage_06_system_event_pressure
    main.c
)

if(MSVC)
    target_compile_options(watch_arch_stage_06_system_event_pressure
        PRIVATE /W4)
else()
    target_compile_options(watch_arch_stage_06_system_event_pressure PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

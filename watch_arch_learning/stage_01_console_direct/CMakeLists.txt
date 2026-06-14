add_executable(watch_arch_stage_01_console_direct
    main.c
)

if(MSVC)
    target_compile_options(watch_arch_stage_01_console_direct PRIVATE /W4)
else()
    target_compile_options(watch_arch_stage_01_console_direct PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

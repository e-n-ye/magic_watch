add_executable(watch_arch_stage_02_context_menu
    main.c
)

if(MSVC)
    target_compile_options(watch_arch_stage_02_context_menu PRIVATE /W4)
else()
    target_compile_options(watch_arch_stage_02_context_menu PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

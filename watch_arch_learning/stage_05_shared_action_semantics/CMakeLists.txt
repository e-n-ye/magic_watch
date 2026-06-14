add_executable(watch_arch_stage_05_shared_action_semantics
    main.c
)

if(MSVC)
    target_compile_options(watch_arch_stage_05_shared_action_semantics
        PRIVATE /W4)
else()
    target_compile_options(watch_arch_stage_05_shared_action_semantics PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

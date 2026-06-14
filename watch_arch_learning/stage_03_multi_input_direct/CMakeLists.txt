add_executable(watch_arch_stage_03_multi_input_direct
    main.c
)

if(MSVC)
    target_compile_options(watch_arch_stage_03_multi_input_direct PRIVATE /W4)
else()
    target_compile_options(watch_arch_stage_03_multi_input_direct PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()


add_executable(tests
    main.c
    test_dcmp.c
    test_math.c
    test_img_gen.c
)

target_compile_options(tests PRIVATE $<$<CONFIG:Debug>:-ggdb>)

target_link_libraries(tests
    PRIVATE img_io
    PRIVATE img
    PRIVATE img_dcmp
    PRIVATE img_math
    PRIVATE img_gen
)

add_test(NAME tests COMMAND tests)

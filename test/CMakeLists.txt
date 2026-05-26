
add_executable(tests
    main.c
    test_dcmp.c
    test_math.c
)

target_compile_options(tests PRIVATE $<$<CONFIG:Debug>:-ggdb>)

target_link_libraries(tests
    PRIVATE io
    PRIVATE image
    PRIVATE dcmp
    PRIVATE img_math
)

add_test(NAME tests COMMAND tests)

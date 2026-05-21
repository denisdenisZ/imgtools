
add_executable(tests
    main.c
    test_dcmp.c
)

target_compile_options(tests PRIVATE $<$<CONFIG:Debug>:-ggdb>)

target_link_libraries(tests
    PRIVATE io
    PRIVATE image
    PRIVATE dcmp
)

add_test(NAME tests COMMAND tests)

add_library(img_gen
    basic_patterns.c
)

target_link_libraries(img_gen
    PRIVATE img
)

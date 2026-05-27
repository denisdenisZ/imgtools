add_library(img_gen
    imgen.c
    basic_patterns.c
    imgfactory.c
)

target_include_directories(img_gen PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(img_gen PUBLIC img PRIVATE img_io img_math)

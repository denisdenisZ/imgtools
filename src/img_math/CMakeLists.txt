add_library(img_math
    img_math.c
)

target_include_directories(img_math PUBLIC ${CMAKE_SOURCE_DIR}/include/math)

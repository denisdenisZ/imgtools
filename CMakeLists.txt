cmake_minimum_required(VERSION 3.20)
project(imgtool C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(io
    src/io.c
)
target_include_directories(io PUBLIC include)

add_library(image
    src/image.c
)
target_include_directories(image PUBLIC include)

add_executable(imgtool
    src/main.c
)
target_link_libraries(imgtool
    PRIVATE io
    PRIVATE image
)

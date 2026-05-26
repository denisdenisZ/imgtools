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

add_library(dcmp
    src/dcmp/dcmp.c
)
target_include_directories(dcmp PUBLIC include/dcmp)
target_include_directories(dcmp PUBLIC include/math)

add_executable(imgtool
    src/main.c
)

add_subdirectory(src/math)

target_link_libraries(imgtool
    PRIVATE io
    PRIVATE image
    PRIVATE dcmp
)

enable_testing()
add_subdirectory(test)

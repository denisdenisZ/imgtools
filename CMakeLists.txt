cmake_minimum_required(VERSION 3.20)
project(imgtool C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src/img)
add_subdirectory(src/img_dcmp)
add_subdirectory(src/img_gen)
add_subdirectory(src/img_io)
add_subdirectory(src/img_math)

add_executable(imgtool src/main.c)
target_link_libraries(imgtool PRIVATE img img_dcmp img_io img_math)

add_subdirectory(src/apps)

enable_testing()
add_subdirectory(test)

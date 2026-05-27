add_library(img_io io.c pgm.c)
target_include_directories(img_io PUBLIC ${CMAKE_SOURCE_DIR}/include/io)
target_link_libraries(img_io PUBLIC img)

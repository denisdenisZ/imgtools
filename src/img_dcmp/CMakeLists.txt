add_library(img_dcmp dcmp.c)
target_include_directories(img_dcmp PUBLIC ${CMAKE_SOURCE_DIR}/include/dcmp)
target_include_directories(img_dcmp PUBLIC ${CMAKE_SOURCE_DIR}/include/math)

cmake_minimum_required(VERSION 3.16)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

aux_source_directory(. LIB_SRC)

add_library(nem_com STATIC ${LIB_SRC})

target_include_directories(nem_com PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
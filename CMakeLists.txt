cmake_minimum_required(VERSION 3.16)
project(SDL3Game)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")

include_directories(include)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

message(STATUS "SRC_FILES: ${SRC_FILES}")

add_executable(main main.c ${SRC_FILES})

target_link_libraries(main PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libSDL2.a
    ${CMAKE_SOURCE_DIR}/lib/libSDL2_image.a
    pthread
    dl
    m
)

cmake_minimum_required(VERSION 3.16)

project(sdl_example)

set(CMAKE_C_STANDARD 99)

FILE(GLOB sources *.c)
FILE(GLOB mcu_renderer_sources ../../src/mcu-renderer/*.c)

find_package(SDL2 CONFIG REQUIRED)

add_definitions(-D MCURENDERER_SDL)

add_executable(sdl_example ${sources} ${mcu_renderer_sources})

target_include_directories(sdl_example PRIVATE ../../src/mcu-renderer ../../fonts)
target_link_libraries(sdl_example
PRIVATE
$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

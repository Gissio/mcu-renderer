cmake_minimum_required(VERSION 3.16)

project(fontconv)

set(CMAKE_CXX_STANDARD 20)
set(VCPKG_TARGET_TRIPLET x64-windows-static)

FILE(GLOB sources *.cpp)

find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

add_executable(fontconv ${sources})

target_link_libraries(fontconv PRIVATE Freetype::Freetype)

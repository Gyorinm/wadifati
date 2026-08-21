cmake_minimum_required(VERSION 3.20)
project(AetheraEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)

add_library(aethera
    src/engine.cpp
    src/physics.cpp
    src/renderer.cpp
)

target_include_directories(aethera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aethera PUBLIC SDL2::SDL2)

target_compile_features(aethera PUBLIC cxx_std_20)

if (MSVC)
    target_compile_options(aethera PRIVATE /W4 /permissive-)
else()
    target_compile_options(aethera PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(aethera_demo examples/demo/main.cpp)
target_link_libraries(aethera_demo PRIVATE aethera)

install(TARGETS aethera aethera_demo)
install(DIRECTORY include/ DESTINATION include)

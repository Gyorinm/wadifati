cmake_minimum_required(VERSION 3.20)
project(AetheraEngine VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(SDL2 REQUIRED)
add_library(aethera
    src/animation_graph.cpp
    src/behavior.cpp
    src/engine.cpp
    src/image.cpp
    src/image_analyzer.cpp
    src/image_object.cpp
    src/image_renderer.cpp
    src/ik.cpp
    src/object.cpp
    src/physics.cpp
    src/procedural_animation.cpp
    src/renderer.cpp
    src/script.cpp
    src/script_expr.cpp
    src/script_runtime.cpp
    src/semantic.cpp
    src/skeleton_physics.cpp
    src/skinning.cpp
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
add_executable(aethera_image_test tests/image_analyzer_test.cpp)
target_link_libraries(aethera_image_test PRIVATE aethera)
add_executable(aethera_semantic_test tests/semantic_test.cpp)
target_link_libraries(aethera_semantic_test PRIVATE aethera)
add_executable(aethera_skeleton_physics_test tests/skeleton_physics_test.cpp)
target_link_libraries(aethera_skeleton_physics_test PRIVATE aethera)
add_executable(aethera_skinning_test tests/skinning_test.cpp)
target_link_libraries(aethera_skinning_test PRIVATE aethera)
add_executable(aethera_ik_test tests/ik_test.cpp)
target_link_libraries(aethera_ik_test PRIVATE aethera)
add_executable(aethera_animation_test tests/procedural_animation_test.cpp)
target_link_libraries(aethera_animation_test PRIVATE aethera)
add_executable(aethera_behavior_test tests/behavior_test.cpp)
target_link_libraries(aethera_behavior_test PRIVATE aethera)
add_executable(aethera_animation_graph_test tests/animation_graph_test.cpp)
target_link_libraries(aethera_animation_graph_test PRIVATE aethera)
add_executable(aethera_script_test tests/script_test.cpp)
target_link_libraries(aethera_script_test PRIVATE aethera)
add_executable(aethera_script_expr_test tests/script_expr_test.cpp)
target_link_libraries(aethera_script_expr_test PRIVATE aethera)
add_executable(aethera_script_physics_test tests/script_physics_test.cpp)
target_link_libraries(aethera_script_physics_test PRIVATE aethera)
install(TARGETS aethera aethera_demo)
install(DIRECTORY include/ DESTINATION include)

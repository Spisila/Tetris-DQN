cmake_minimum_required(VERSION 3.10)

project(GEORGE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)

FetchContent_MakeAvailable(pybind11)
FetchContent_MakeAvailable(raylib)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/rl
)

set(CORE_SOURCES
    src/core/game.cpp
    include/core/game.hpp
    src/core/board.cpp
    include/core/board.hpp
    src/core/tetromino.cpp
    include/core/tetromino.hpp
    src/core/piece_queue.cpp
    include/core/piece_queue.hpp
    include/core/types.hpp
)

add_executable(GEORGE 
    src/main.cpp
    ${CORE_SOURCES}
    src/core/game_renderer.cpp

)


target_link_libraries(GEORGE PRIVATE raylib)

include(GoogleTest)

add_executable(GEORGE_TESTS
    tests/board_test.cpp
    ${CORE_SOURCES}
)

target_link_libraries(GEORGE_TESTS PRIVATE raylib gtest_main)

gtest_discover_tests(GEORGE_TESTS)

pybind11_add_module(Tetris_AGENT 
    include/rl/rl_env.hpp
    src/rl/rl_env.cpp
    src/core/game_renderer.cpp
    src/rl/multi_game.cpp
    ${CORE_SOURCES}
    src/bindings.cpp
)

target_link_libraries(Tetris_AGENT PRIVATE raylib)

set_target_properties(Tetris_AGENT PROPERTIES
    PREFIX ""
    SUFFIX ".pyd"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

set_target_properties(GEORGE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

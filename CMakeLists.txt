cmake_minimum_required(VERSION 3.28)
project(ants LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 23)
add_executable(
    ants
    ./src/main.cpp 
    ./src/Ant.cpp 
    ./src/Id.cpp 
    ./src/MovementStrategy.cpp
    ./src/Timer.cpp
    ./src/Visualizer.cpp
    ./src/World.cpp
)
target_compile_features(ants PRIVATE cxx_std_23)
target_link_libraries(ants PRIVATE SFML::Graphics)
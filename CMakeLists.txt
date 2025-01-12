cmake_minimum_required(VERSION 3.28)

project(Sand-cpp LANGUAGES CXX)

add_executable(sand-cpp 
    src/main.cpp
    src/Screen.cpp
    src/WorldState.cpp
    src/SandWorld.cpp
    src/Cell.cpp
    src/Chunks.cpp
    src/Helpers.cpp
    src/Elements/ElementProperties.cpp
    src/Elements/PropertiesContainer.cpp
    src/Elements/Solid.cpp
    src/Elements/Liquid.cpp
    src/Elements/Gas.cpp)

target_include_directories(sand-cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/inc
    PRIVATE ${PROJECT_SOURCE_DIR}/inc/Elements)
target_compile_features(sand-cpp PRIVATE cxx_std_17)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

target_link_libraries(sand-cpp 
    PRIVATE sfml-system 
    PRIVATE sfml-graphics)
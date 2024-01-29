# Personalized version of the CMakeLists of the following source
# https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

project(BUas_Intake)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_executable(${CMAKE_PROJECT_NAME} ${CPP_SOURCES})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./res/")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-audio sfml-network sfml-system sfml-window)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

install(TARGETS ${CMAKE_PROJECT_NAME})
# Personalized version of the CMakeLists of the following source
# https://github.com/meemknight/learnCmake/blob/master/moreCompleteExample/Cmakelists.txt

cmake_minimum_required(VERSION 3.20)

set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug$<$<CONFIG:Debug>:Debug>)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Release>:Release>)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lsfml-graphics -lsfml-window -lsfml-system")

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

project(BUas_Intake)
set(CMAKE_CXX_STANDARD 17)
add_executable(${CMAKE_PROJECT_NAME} ${CPP_SOURCES})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./res/")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE lib)
target_link_libraries(${CMAKE_PROJECT_NAME} sfml-audio sfml-network sfml-system sfml-window)
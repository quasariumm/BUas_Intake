# Personalized version of the CMakeLists of the following source
# https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt
# Added some SFML config from https://github.com/SFML/SFML/issues/2289 (the author's CMakeLists.txt)

cmake_minimum_required(VERSION 3.20)

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_USE_STATIC_STD_LIBS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
set(SFML_ENABLE_PCH ON CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_FRAMEWORKS OFF CACHE BOOL "" FORCE)
set(SFML_STATIC_LIBRARIES ON CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

project(BUas_Intake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${CMAKE_PROJECT_NAME} ${CPP_SOURCES})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./res/")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE lib)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-network sfml-window sfml-system)

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

install(TARGETS ${CMAKE_PROJECT_NAME})
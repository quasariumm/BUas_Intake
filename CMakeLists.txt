# Personalized version of the CMakeLists of the following source
# https://github.com/SFML/cmake-sfml-project/blob/master/CMakeLists.txt
# Added some SFML config from https://github.com/SFML/SFML/issues/2289 (the author's CMakeLists.txt)

cmake_minimum_required(VERSION 3.20)

include(FetchContent)

set(SFML_USE_STATIC_STD_LIBS OFF)
set(SFML_BUILD_WINDOW ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_NETWORK OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_TEST_SUITE OFF)
set(SFML_ENABLE_COVERAGE OFF)
set(SFML_INSTALL_PKGCONFIG_FILES OFF)
set(SFML_ENABLE_PCH OFF)
set(SFML_USE_SYSTEM_DEPS OFF)
set(SFML_BUILD_FRAMEWORKS OFF)
set(SFML_BUILD_STATIC_LIBS OFF)
set(BUILD_SHARED_LIBS ON)

# Don't make pdb files
set(SFML_GENERATE_PDB OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML)

file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

project(SorryWereBroke)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (WIN32)
	add_executable(${CMAKE_PROJECT_NAME} WIN32 ${CPP_SOURCES})
else()
	add_executable(${CMAKE_PROJECT_NAME} ${CPP_SOURCES})
endif()

#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
#target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/")
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./res/")
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC DATA_PATH="./data/")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_directories(${CMAKE_PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib
)

# Add openal32 to the libs
if (MSVC OR WIN32)
	file(COPY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML/extlibs/bin/x64/openal32.dll ${CMAKE_CURRENT_BINARY_DIR}/lib/openal32.dll)
	file(COPY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lib/SFML/extlibs/libs-msvc-universal/x64/openal32.lib ${CMAKE_CURRENT_BINARY_DIR}/lib/openal32.lib)
elseif(UNIX)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/openal/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/.)
endif()

# Add SFML DLLs to PATH
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib/SFML/bin/)

if (UNIX)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system openal)
elseif(WIN32 OR MSVC)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system)
endif()

if(WIN32 OR MSVC)
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

# Add the data and res folder to the executable folder
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_CURRENT_BINARY_DIR}/res
	COMMAND ${CMAKE_COMMAND} -E remove_directory
		${CMAKE_CURRENT_BINARY_DIR}/data
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/res
		${CMAKE_CURRENT_BINARY_DIR}/res
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/data
		${CMAKE_CURRENT_BINARY_DIR}/data
)

# After SFML build completes, copy the DLLs to the binary directory
if (WIN32 OR MSVC)
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  	  COMMAND ${CMAKE_COMMAND} -E copy_directory
  	  ${CMAKE_CURRENT_BINARY_DIR}/lib/SFML/bin/
  	  ${CMAKE_CURRENT_BINARY_DIR}
	)
elseif(UNIX)
	add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_BINARY_DIR}/lib/SFML/lib/
		${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME})
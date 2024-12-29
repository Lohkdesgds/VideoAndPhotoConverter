cmake_minimum_required(VERSION 3.28)
project(vpc_gui LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#find_package(Freetype REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype) # since CMake 3.10

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)


#cmake_minimum_required(VERSION 3.22)
#
#if (DEFINED $ENV{CMAKE_TOOLCHAIN_FILE})
#    include ($ENV{CMAKE_TOOLCHAIN_FILE})
#endif()
#
#project(vpc_gui VERSION 1.0)
#
## Release?
##set(CMAKE_BUILD_TYPE Release)
#
#if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
#    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "_DEBUG")
#else ()
#    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "NDEBUG")
#endif ()
#
#
#file(GLOB_RECURSE source_list "src/*.cpp")
#add_executable(${PROJECT_NAME} ${source_list})
#
#target_include_directories(${PROJECT_NAME} PRIVATE 
#    "deps"
#)
#
## Set C++ version
#set_target_properties(${PROJECT_NAME} PROPERTIES
#    CXX_STANDARD 20
#    CXX_STANDARD_REQUIRED ON
#)
#
#find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
#target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)

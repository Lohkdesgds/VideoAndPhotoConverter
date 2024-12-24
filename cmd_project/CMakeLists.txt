cmake_minimum_required(VERSION 3.15)

if (DEFINED $ENV{CMAKE_TOOLCHAIN_FILE})
    include ($ENV{CMAKE_TOOLCHAIN_FILE})
endif()

project(vpc_cmd VERSION 1.0)

# Release?
set(CMAKE_BUILD_TYPE Release)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "_DEBUG")
else ()
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "NDEBUG")
endif ()


file(GLOB_RECURSE source_list "src/*.cpp")
add_executable(${PROJECT_NAME} ${source_list})

target_include_directories(${PROJECT_NAME} PRIVATE 
    "deps"
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

find_package( CURL REQUIRED )
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl )

add_custom_command(TARGET ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ../${PROJECT_NAME})
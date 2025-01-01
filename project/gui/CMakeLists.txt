cmake_minimum_required(VERSION 3.28)
project(vpc_gui LANGUAGES CXX)

get_filename_component(SHARED_DIR ../shared ABSOLUTE)

set(SFML_STATIC_LIBRARIES TRUE)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

get_filename_component(SHARED_DIR ../shared ABSOLUTE)

file(GLOB_RECURSE source_list "src/*.cpp" ${SHARED_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${source_list})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SHARED_DIR}/deps
    ${SHARED_DIR}/include
    deps
    include
)

target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Network SFML::System SFML::Window SFML::Audio)
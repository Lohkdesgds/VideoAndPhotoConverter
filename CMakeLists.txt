cmake_minimum_required(VERSION 3.22)

# Did not work (?)
if (MSVC)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE
    ${CMAKE_CURRENT_SOURCE_DIR}/msvc_overrides/c_flag_overrides.cmake)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
    ${CMAKE_CURRENT_SOURCE_DIR}/msvc_overrides/cxx_flag_overrides.cmake)
endif()

project(VideoAndPhotoConverterCombo)

add_subdirectory(project/cmd)

# Linux: needs sudo apt install libxcursor-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libopengl-dev 
## commenting temporarily
#add_subdirectory(project/gui)

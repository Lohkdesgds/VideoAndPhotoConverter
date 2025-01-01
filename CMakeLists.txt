cmake_minimum_required(VERSION 3.22)

#set(VCPKG_VERSION latest)
#include(vcpkg_integration/vcpkg.cmake)

project(VideoAndPhotoConverterCombo)

add_subdirectory(project/cmd)

# Linux: needs sudo apt install libxcursor-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libopengl-dev 
add_subdirectory(project/gui)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

project(VideoAndPhotoConverterCombo)

add_subdirectory(project/cmd)

# Linux: needs sudo apt install libxcursor-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libopengl-dev 
## commenting temporarily
#add_subdirectory(project/gui)

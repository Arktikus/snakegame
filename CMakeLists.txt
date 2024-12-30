cmake_minimum_required(VERSION 3.15)
project(SnakeGame)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Raylib
include_directories("C:/msys64/mingw64/include")   # Include Raylib headers
link_directories("C:/msys64/mingw64/lib")          # Link Raylib libraries

# Add Executable
add_executable(${PROJECT_NAME} WIN32 src/main.cpp) # WIN32 for Windows because console was opening.

# Link Raylib (adjust for your compiler)
if(MSVC)
    target_link_libraries(${PROJECT_NAME} raylib.lib)
else()
    target_link_libraries(${PROJECT_NAME} raylib)
endif()

# Ensure MinGW Links Win32 Libraries
if(MINGW)
    target_link_libraries(${PROJECT_NAME} gdi32 winmm)
endif()

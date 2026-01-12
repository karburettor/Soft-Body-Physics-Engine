cmake_minimum_required(VERSION 3.10)
project(SoftBodyPhysics)

set(CMAKE_CXX_STANDARD 17)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Include directories
include_directories(include)

# Create executable
add_executable(SoftBodyPhysics src/main.cpp)

# Link SFML
target_link_libraries(SoftBodyPhysics sfml-graphics sfml-window sfml-system)
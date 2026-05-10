cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0072 NEW)
project(GraphicsProject)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(main
    src/main.cpp
    src/model.cpp
)

target_link_libraries(main
    OpenGL::GL
    assimp::assimp
    glfw
    GLEW::GLEW
)

# Mac: suppress deprecated OpenGL warnings (Apple deprecated OpenGL in 10.14)
if(APPLE)
    target_compile_definitions(main PRIVATE GL_SILENCE_DEPRECATION)
    # Homebrew installs to /opt/homebrew on Apple Silicon, /usr/local on Intel
    if(EXISTS /opt/homebrew)
        include_directories(/opt/homebrew/include)
        link_directories(/opt/homebrew/lib)
    else()
        include_directories(/usr/local/include)
        link_directories(/usr/local/lib)
    endif()
endif()

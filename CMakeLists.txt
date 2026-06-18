cmake_minimum_required(VERSION 3.20)
project(ArenaShooter)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/enet)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)

find_package(OpenGL REQUIRED)

add_executable(client
    client/client.cpp
    third_party/glad/src/glad.c
    Src/Entity.cpp
    Src/Map.cpp
    Src/PowerUp.cpp
    Src/Power_Ups.cpp
    Src/Tile.cpp
    Src/Tiles.cpp
    Src/EBO.cpp
    Src/VAO.cpp
    Src/VBO.cpp
    Src/ShaderClass.cpp
)


target_include_directories(client PRIVATE
    third_party/glad/include
    third_party/enet/include
    Include
)


target_link_libraries(client PRIVATE
    glfw
    glm::glm
    OpenGL::GL
    enet
    dl
    pthread
)
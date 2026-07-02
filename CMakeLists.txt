cmake_minimum_required(VERSION 3.20)
project(ArenaShooter)

set(CMAKE_CXX_STANDARD 20)

#add_subdirectory(third_party/enet)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)

find_package(OpenGL REQUIRED)

add_executable(client
    client/main.cpp
    third_party/glad/src/glad.c
    src/Entity.cpp
    src/Map.cpp
    src/PowerUp.cpp
    src/Power_Ups.cpp
    src/Tile.cpp
    src/EBO.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/ShaderClass.cpp
    src/Tile_Manager.cpp
    src/CollisionChecker.cpp
)


target_include_directories(client PRIVATE
    third_party/glad/include
    third_party/enet/include
    include
)


target_link_libraries(client PRIVATE
    glfw
    glm::glm
    OpenGL::GL
    enet
    dl
    pthread
)
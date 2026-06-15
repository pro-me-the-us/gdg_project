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
    
)

add_executable(server
    server/server.cpp
)

target_include_directories(client PRIVATE
    third_party/glad/include
    third_party/enet/include
)

target_include_directories(server PRIVATE
    third_party/enet/include
)

target_link_libraries(client PRIVATE
    glfw
    glm::glm
    OpenGL::GL
    enet
    dl
    pthread
)

target_link_libraries(server PRIVATE
    enet
    dl
    pthread
)
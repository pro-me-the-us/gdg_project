cmake_minimum_required(VERSION 3.20)
project(ArenaShooter)

set(CMAKE_CXX_STANDARD 20)

# collect enet source files from correct path
file(GLOB ENET_SOURCES "third_party/enet/enet/*.c")

add_subdirectory(third_party/glfw)
add_subdirectory(third_party/glm)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

add_executable(client
    client/imgui_main.cpp
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
    src/Bullet.cpp
    src/Bullet_Manager.cpp
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_impl_glfw.cpp
    third_party/imgui/imgui_impl_opengl3.cpp
    third_party/imgui/imgui_demo.cpp
    ${ENET_SOURCES}
)

target_include_directories(client PRIVATE
    third_party/glad/include
    third_party/enet/enet/include
    include
)

target_link_libraries(client PRIVATE
    glfw
    glm::glm
    OpenGL::GL
    dl
    pthread
)

# windows needs extra socket libraries
if(WIN32)
    target_link_libraries(client PRIVATE ws2_32 winmm)
endif()
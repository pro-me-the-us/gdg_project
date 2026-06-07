cmake_minimum_required(VERSION 3.20)

project(ArenaShooter)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
# find_package(enet CONFIG REQUIRED)



add_executable(client
    client/main.cpp
    third_party/glad/src/glad.c
)

target_include_directories(client PRIVATE
    third_party/glad/include
)

target_link_libraries(client PRIVATE
    glfw
    glm::glm
    # enet::enet
)
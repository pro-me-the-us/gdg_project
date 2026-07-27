#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <cmath>
#include <vector>
#include "Entity.h"
#include "Tile.h"
#include "Tile_Manager.h"
#include "Map.h"
#include "CollisionChecker.h"
#include "networking.h"
#include "Bullet.h"
#include "Bullet_Manager.h"
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_impl_glfw.h"
#include "../third_party/imgui/imgui_impl_opengl3.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error " << error << ": " << description << "\n";
}

int main(int, char**)
{
    // 1. Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // Hardcode to OpenGL 3.3 Core (Standard for modern desktop)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create Window
    GLFWwindow* window = glfwCreateWindow(1280, 800, "Game Launcher", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // 3. Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // 4. Setup Dear ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // 5. Setup Platform/Renderer Backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Background color for the window (Dark Grey)
    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // 6. Main Render Loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // =======================================================
        // YOUR UI CODE GOES HERE
        // =======================================================
        
        ImGui::Begin("Join Game");
        ImGui::Text("Welcome to the Arena.");
        
        // You will add your ImGui::InputText and ImGui::Button here!
        
        ImGui::End();

        // =======================================================

        // 7. Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // 8. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
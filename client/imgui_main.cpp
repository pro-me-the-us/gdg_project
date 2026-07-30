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


GLuint loadTexture(const char *path, const char *name)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &w, &h, &ch, 0);
    std::cout << name << " Texture: " << (data ? "OK" : "FAILED") << std::endl;
    std::cout << "W:" << w << " H:" << h << " CH:" << ch << std::endl;
    if (!data)
    {
        std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
        return -1;
    }
    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    return texture;
}

enum class screenStates{
    mainRoom,
    joinRoom,
    createRoom,
    gameRoom
};

int main()
{
    bool open = true;
    static char nameBuffer[64] = "";
    static char roomIDBuffer[64]="";
    
    // 1. Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // Hardcode to OpenGL 3.3 Core (Standard for modern desktop)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create Window
    GLFWwindow* window = glfwCreateWindow(768, 576, "Game Launcher", nullptr, nullptr);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImFont* mainfont = io.Fonts -> AddFontFromFileTTF("../resources/Fonts/Pixel.ttf",18.0f);

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330 core");


    
    GLuint jsTexture = loadTexture("../resources/JoinSRC/joinSRC.png","Join Screen");
    
    screenStates currentScreen = screenStates::mainRoom;
    

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.07f,0.13f,0.17f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(768,576));
        ImGui::SetNextWindowPos(ImVec2(0,0));
        
        ImGui::Begin("JoinGame Window",&open,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowBottomRight = ImVec2(windowPos.x + windowSize.x,windowPos.y+windowSize.y);
        
        ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)jsTexture,windowPos,windowBottomRight,ImVec2(0.0f,1.0f),ImVec2(1.0f,0.0f));
        
        ImGui::PushFont(mainfont);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        switch(currentScreen){
            case screenStates::mainRoom:{
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x/2 - ImGui::CalcTextSize("Enter Your GameTag").x/2);
                ImGui::Text("Enter Your GameTag");
        
                ImGui::InputText("GameTag",nameBuffer,sizeof(nameBuffer));
                        
                if(ImGui::Button("Join Room")){
                //button returns true so do the whole save the username-terminate window-change concext thing
                    currentScreen = screenStates::joinRoom;
                }

                ImGui::SameLine(0.0f,60.0f);

                if(ImGui::Button("Create New Room")){
                    currentScreen = screenStates::createRoom;
                }

                break;
            }


            case screenStates::joinRoom:{
                ImGui::Text("Enter Room-ID");
                ImGui::InputText("Room-ID",roomIDBuffer,sizeof(roomIDBuffer));
                if(ImGui::Button("Start Game")){
                    currentScreen = screenStates::gameRoom;
                }                
                break;
            }


            case screenStates::createRoom:{
                //generate the roomID as per ENET requirement
                //set the room details such as player numbers, etc
                //launch the gameRoom

                currentScreen = screenStates::gameRoom;
                break;
            }

            case screenStates::gameRoom:{
                //break out of this window and launch the gameroom
                break;
            }
        }

        ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::cout<<nameBuffer;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
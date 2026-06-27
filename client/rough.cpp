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
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <cmath>
#include <vector>
#include "ShaderClass.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ORIGINAL_TILE_SIZE 16
#define SCALE 3
#define TILE_SIZE ORIGINAL_TILE_SIZE*SCALE
#define MAX_SCREEN_COL 16
#define MAX_SCREEN_ROW 12
#define VELOCITY 5.0f
#define FPS 60


GLuint loadTexture(const char* path, const char* name)
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
	unsigned char* data = stbi_load(path, &w, &h, &ch, 0);
	std::cout << name << " Texture: " << (data ? "OK" : "FAILED") << std::endl;
	std::cout << "W:" << w << " H:" << h << " CH:" << ch << std::endl;
	if (!data) {
		std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
		return -1;
	}
	if (!data) {
		std::cout << "Failed to load texture: " << path << std::endl;
		return 0;
	}

	GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	return texture;
}

void draw(Shader& shaderProgram, VAO& VAO1, GLuint texture)
{
	shaderProgram.Activate();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "u_Texture"), 0);

    VAO1.Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float WindowWidth = TILE_SIZE * MAX_SCREEN_COL;
	float WindowHeight = TILE_SIZE * MAX_SCREEN_ROW;

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "FirstWin", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Window Creation Failed" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WindowWidth, WindowHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float spriteW = 0.5f;  // actual sprite width
	float spriteH = 0.5f; // actual sprite height

	std::vector<GLfloat> vertices =
	{
	   -spriteW,  spriteH,  0.0f,  0.0f, 1.0f,
		spriteW,  spriteH,  0.0f,  1.0f, 1.0f,
		spriteW, -spriteH,  0.0f,  1.0f, 0.0f,
	   -spriteW, -spriteH,  0.0f,  0.0f, 0.0f
	};

	std::vector<GLuint> indices = { 0,1,2, 2,3,0 };

	Shader shaderProgram("../resources/shaders/default.vert", "../resources/shaders/default.frag");

	VAO VAO1;
	VAO1.Bind();
	VBO VBO1(vertices, vertices.size() * sizeof(GLfloat));
	EBO EBO1(indices, indices.size() * sizeof(GLuint));
	VAO1.LinkVBO(VBO1, 0, 3, 5 * sizeof(GLfloat), (void*)0);
	VAO1.LinkVBO(VBO1, 1, 2, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	int scale_loc = glGetUniformLocation(shaderProgram.ID,"u_UVscale");
	int offset_loc = glGetUniformLocation(shaderProgram.ID, "u_UVoffset");

    unsigned int player_texture = loadTexture("../resources/sprites/spritesheet/down.png","down");

	glm::vec2 scale = glm::vec2(0.5f,0.5f);
    glm::vec2 offset = glm::vec2(0.0f,0.0f);


    double drawInterval = 1000000000 / FPS;
	double delta = 0;
	long timer = 0;
	long drawCount = 0;

    auto lastTime = std::chrono::steady_clock::now();

    //1-top left
    //2-top right
    //3-bottom left
    //4-bottom right
    int currID = 1;
    
    //1-up
    //2-down
    //3-left
    //4-right
    int direction_ID=1;


	glClearColor(0.3f, 0.5f, 1.0f, 1.0f);
	while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = currentTime - lastTime;
		delta += elapsedTime.count() / drawInterval;
		timer += elapsedTime.count();
		lastTime = currentTime;


        shaderProgram.Activate(); 
        glUniform2f(scale_loc, scale.x, scale.y);
        glUniform2f(offset_loc, offset.x, offset.y);
        draw(shaderProgram,VAO1,player_texture);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

        if (delta >= 1)
		{
            if(direction_ID==1){
                player_texture=loadTexture("../resources/sprites/spritesheet/up.png","up");
            }
            else if(direction_ID==2){
                player_texture=loadTexture("../resources/sprites/spritesheet/down.png","down");
            }
            else if(direction_ID==3){
                player_texture=loadTexture("../resources/sprites/spritesheet/left.png","left");
            }
            else if(direction_ID==4){
                player_texture=loadTexture("../resources/sprites/spritesheet/right.png","right");
            }

			if(drawCount%10==0){
                if (currID == 1) {
                    // 1 - Top Left
                    offset.x = 0.0f;
                    offset.y = 0.0f; 
                    currID++;
                }
                else if (currID == 2) {
                    // 2 - Top Right
                    offset.x = 0.5f;
                    offset.y = 0.0f;
                    currID++;
                }
                else if (currID == 3) {
                    // 3 - Bottom Left
                    offset.x = 0.0f;
                    offset.y = 0.5f;
                    currID++;
                }
                else if (currID == 4) {
                    // 4 - Bottom Right
                    offset.x = 0.5f;
                    offset.y = 0.5f;
                    currID = 1; // Reset to loop the animation
                }
            }
			delta--;
			drawCount++;
		}

		if (timer >= 1000000000)
		{
			std::cout << "FPS : " << drawCount << "\n";
			drawCount = 0;
			timer = 0;
		}
	}

	VAO1.Delete();
	VBO1.Delete();
	glfwTerminate();
	return 0;
}
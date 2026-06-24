#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ORIGINAL_TILE_SIZE 16
#define SCALE 3
#define TILE_SIZE ORIGINAL_TILE_SIZE*SCALE
#define MAX_SCREEN_COL 16
#define MAX_SCREEN_ROW 12
#define VELOCITY 0.01f
#define FPS 60


void handleInput(GLFWwindow* window, Entity* Player)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Player->attriby += VELOCITY;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Player->attriby -= VELOCITY;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Player->attribx += VELOCITY;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Player->attribx -= VELOCITY;
	
}


void draw(Shader& shaderProgram, VAO& VAO1, GLuint uniID, GLuint texture,Entity* Player)
{
	glClearColor(0.3f, 0.2f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shaderProgram.Activate();
	glUniform2f(uniID, Player->attribx, Player->attriby);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "u_Texture"), 0);

	VAO1.Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void run(GLFWwindow* window, Shader shaderProgram, VAO VAO1, GLuint uniID, GLuint texture,Entity* Player)
{
	double drawInterval = 1000000000 / FPS;
	double delta = 0;
	long timer = 0;
	long drawCount = 0;

	auto lastTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = currentTime - lastTime;
		delta += elapsedTime.count() / drawInterval;
		timer += elapsedTime.count();
		lastTime = currentTime;

		draw(shaderProgram, VAO1, uniID, texture,Player);
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (delta >= 1)
		{
			handleInput(window, Player);
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
}


GLuint loadTexture(const char* path)
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
	std::cout << "Texture: " << (data ? "OK" : "FAILED") << std::endl;
	std::cout << "W:" << w << " H:" << h << " CH:" << ch << std::endl;
	if (!data){
		std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
		return -1;
	}
	if (!data){
		std::cout << "Failed to load texture: " << path << std::endl;
		return 0;
	}

	GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	return texture;
}

void Draw_Map(const char* path,Map& map){
	std::ifstream file(path);
	std::string line;

	if (!file.is_open()) 
    {
        std::cout << "Failed To Open File\n";
        return;
    }

	while (std::getline(file, line)) 
    {
        std::vector<int> currentRow;
        std::stringstream ss(line);
        int tile_ID;
        while (ss >> tile_ID) 
        {
            currentRow.push_back(tile_ID);
        }
        map.mapTileNumber.push_back(currentRow);
    }
    file.close();
}

int main()
{	
	

	Entity* Player = new Entity();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float WindowWidth  = TILE_SIZE * MAX_SCREEN_COL;
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

	float spriteW = 82.0f / WindowWidth;  // actual sprite width
    float spriteH = 80.0f / WindowHeight; // actual sprite height

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

	Tile floor = Tile(0);
	Tile wall = Tile(1);
	floor.texture_ID = loadTexture("../resources/sprites/floor.png");
	wall.texture_ID = loadTexture("../resources/sprites/wall.png");

	Tile_Manager tile_manager;
	tile_manager.tiles.push_back(floor);
	tile_manager.tiles.push_back(wall);

	Map map;
	Draw_Map("../resources/Map/map.txt",map);

	GLuint Player_texture = loadTexture("../resources/sprites/player.png");
	if (Player_texture == 0) return -1;

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "u_Pos");

	run(window, shaderProgram, VAO1, uniID, Player_texture,Player);

	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();
	glfwTerminate();
	delete(Player);
	return 0;
}
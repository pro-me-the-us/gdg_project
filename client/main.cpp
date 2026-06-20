#include <iostream>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <cmath>
#include <vector>
#define ORIGINAL_TILE_SIZE 16
#define SCALE 3
#define TILE_SIZE ORIGINAL_TILE_SIZE*SCALE
#define MAX_SCREEN_COL 16
#define MAX_SCREEN_ROW 12
#define VELOCITY 0.007f
#define FPS 30

/*
    //Screen settings
    int originalTileSize = 16; //16x16 tiles i.e size of player
    int scale = 3;
    int tileSize = originalTileSize*scale; //Actual tile size
    int maxScreenCol= 16;
    int maxScreenRow = 12;
    int screenWidth = tileSize*maxScreenCol; //768 pixels
    int screenHeight = tileSize*maxScreenRow; //576 pixels

    //World Setting
    int maxWorldCol = 65;
    int maxWorldRow =65;
    int worldWidth = tileSize*maxWorldCol;
    int worldHeight = tileSize*maxWorldRow;

*/


void run(GLFWwindow* window,Shader shaderProgram,VAO VAO1,GLuint uniID){

	double drawInterval = 1000000000/FPS;

	double delta=0;
	std::chrono::time_point<std::chrono::steady_clock> lastTime;
	lastTime = std::chrono::steady_clock::now();

	std::chrono::time_point<std::chrono::steady_clock> currentTime;

	long timer = 0;
	long drawCount = 0;
	
	float accumPosx = 0.0f;
	float accumPosy = 0.0f;

	while(!glfwWindowShouldClose(window)){
		currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = currentTime-lastTime;
		delta += elapsedTime.count() / drawInterval;
		timer += elapsedTime.count();
		lastTime = currentTime;

		bool moveup = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		bool movedown = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		bool moveright = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		bool moveleft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;

		glClearColor(0.3f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		glUniform2f(uniID, accumPosx, accumPosy);
		

		VAO1.Bind();
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
		glfwSwapBuffers(window);
		
		//pol for and process event
		glfwPollEvents();

		if(delta>=1){
			if(moveup) accumPosy+=VELOCITY;
			if(movedown) accumPosy-=VELOCITY;
			if(moveright) accumPosx+=VELOCITY;
			if(moveleft) accumPosx-=VELOCITY;

			delta--;
			drawCount++;
		}

		if(timer>=1000000000){
			std::cout<<"FPS : "<<drawCount<<"\n";
			drawCount=0;
			timer=0;
		}
	}
}


int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float WindowWidth = TILE_SIZE*MAX_SCREEN_COL;
	float WindowHeight = TILE_SIZE*MAX_SCREEN_ROW;
	float AspectRatio = WindowWidth / WindowHeight;
	//define the window with it's properties
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "FirstWin", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window Creation Failed" << std::endl;
		return -1;
	}

	float length = (TILE_SIZE/2.0f)/WindowWidth;
	float height = (TILE_SIZE/2.0f)/WindowHeight;

	//vertices of the triangle
	std::vector<GLfloat> vertices = 
    {
        -length,   height,  0.0f,  // Top-Left
        length,   height,  0.0f,  // Top-Right
        length,  -height,  0.0f,  // Bottom-Right
        -length,  -height,  0.0f   // Bottom-Left
    };
	
    std::vector<GLuint> indices = 
    {
        0,1,2,
        2,3,0
    };
    

	//make the window's context curretn
	glfwMakeContextCurrent(window);
	
	//load OpenGL data
	gladLoadGL();

	//tells OpenGL which area to fill the defined color
	glViewport(0, 0, WindowWidth, WindowHeight);
	

	//creating an empty container to store future bullshit that we do
	//vertexshader stores the location of the container
	Shader shaderProgram("../resources/shaders/default.vert", "../resources/shaders/default.frag");

	VAO VAO1;
	VAO1.Bind();
	
	VBO VBO1(vertices, vertices.size()*sizeof(GLfloat));
    EBO EBO1(indices,sizeof(indices)*sizeof(GLuint));
    
	VAO1.LinkVBO(VBO1, 0);
	VAO1.Unbind();
	VBO1.Unbind();
    EBO1.Unbind();
    

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "u_Pos");

	//define the color
	glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
	//set the color to a back buffer
	glClear(GL_COLOR_BUFFER_BIT);
	//swap front and back buffer
	glfwSwapBuffers(window);


	float lastFrameTime = 0.0f;
	float DeltaTime = 0.0f;
	
	run(window,shaderProgram,VAO1,uniID);

	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();
	glfwTerminate();
	
	return 0;
}




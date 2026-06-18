#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <cmath>
#include <vector>

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

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float WindowWidth = 800.0f;
	float WindowHeight = 800.0f;
	float AspectRatio = WindowWidth / WindowHeight;
	//define the window with it's properties
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "FirstWin", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window Creation Failed" << std::endl;
		return -1;
	}

    float halfpxl = 8.0f/800.0f;

	//vertices of the triangle
	std::vector<GLfloat> vertices = 
    {
        -halfpxl,   halfpxl,  0.0f,  // Top-Left
        halfpxl,   halfpxl,  0.0f,  // Top-Right
        halfpxl,  -halfpxl,  0.0f,  // Bottom-Right
        -halfpxl,  -halfpxl,  0.0f   // Bottom-Left
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
	float accumPosx = 0.0f;
	float accumPosy = 0.0f;
	while(!glfwWindowShouldClose(window)){

		float currFrameTime = glfwGetTime();
		DeltaTime = currFrameTime - lastFrameTime;


		//MATH GOES HERE
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			float movey = 0.3f * DeltaTime;
			accumPosy += movey;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			float movey = 0.3f * DeltaTime;
			accumPosy -= movey;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			float movex = 0.3f * DeltaTime;
			accumPosx += movex;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			float movex = 0.3f * DeltaTime;
			accumPosx -= movex;
		}
		glClearColor(0.3f, 0.2f, 0.8f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		glUniform2f(uniID, accumPosx, accumPosy);
		

		VAO1.Bind();
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
		glfwSwapBuffers(window);
		
		//pol for and process event
		glfwPollEvents();
		lastFrameTime = currFrameTime;
	}



	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();
	glfwTerminate();
	return 0;
}
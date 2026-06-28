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
//start

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ORIGINAL_TILE_SIZE 16
#define SCALE 3
#define TILE_SIZE ORIGINAL_TILE_SIZE*SCALE
#define MAX_SCREEN_COL 16
#define MAX_SCREEN_ROW 12
#define VELOCITY 5.0f
#define FPS 60


void handleInput(GLFWwindow* window, Entity* player,int& dirID)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){ 
		player->attriby += player->vely;
		dirID = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){ 
		player->attriby -= player->vely;
		dirID = 2;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){ 
		player->attribx += player->velx;
		dirID = 4;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){ 
		player->attribx -= player->velx;
		dirID = 3;
	}
	
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

GLuint loadTexture(const char* path,const char* name)
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
	std::cout <<name<<" Texture: "<< (data ? "OK" : "FAILED") << std::endl;
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


void run(GLFWwindow* window, Shader& shaderProgram, VAO& VAO1, GLuint texture,Entity* Player,Map& map,Tile_Manager& tile_manager)
{
	double drawInterval = 1000000000 / FPS;
	double delta = 0;
	long timer = 0;
	long drawCount = 0;

	auto lastTime = std::chrono::steady_clock::now();

	glm::mat4 projection = glm::ortho(0.0f,768.0f,0.0f,576.0f,1.0f,-1.0f);
	shaderProgram.Activate();
	int proj_loc = glGetUniformLocation(shaderProgram.ID,"u_Projection");
	int view_loc = glGetUniformLocation(shaderProgram.ID,"u_View");
	int model_loc = glGetUniformLocation(shaderProgram.ID,"u_Model");
	std::cout<<"Projection : "<<proj_loc<<"    Model : "<<model_loc<<"    View : "<<view_loc<<std::endl;
	glUniformMatrix4fv(proj_loc,1,GL_FALSE,glm::value_ptr(projection));

	float tileWorldX = MAX_SCREEN_COL * TILE_SIZE;
	float tileWorldY = MAX_SCREEN_ROW * TILE_SIZE;
	
	glClearColor(0.3f, 0.2f, 0.8f, 1.0f);
	
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

	float curr_off_x = 0.0f;
	float curr_off_y = 0.0f;
	
	int scale_loc = glGetUniformLocation(shaderProgram.ID,"u_UVscale");
	int offset_loc = glGetUniformLocation(shaderProgram.ID, "u_UVoffset");

	GLuint Up = loadTexture("../resources/sprites/spritesheet/up.png", "up");
    GLuint Down = loadTexture("../resources/sprites/spritesheet/down.png", "down");
    GLuint Left = loadTexture("../resources/sprites/spritesheet/left.png", "left");
    GLuint Right = loadTexture("../resources/sprites/spritesheet/right.png", "right");

	GLuint curr_tex = Up;

	glm::vec2 scale;
    glm::vec2 offset;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = currentTime - lastTime;
		delta += elapsedTime.count() / drawInterval;
		timer += elapsedTime.count();
		lastTime = currentTime;
		float Camx = (Player->attribx)-(TILE_SIZE * MAX_SCREEN_COL)/2;
		float Camy = (Player->attriby)-(TILE_SIZE * MAX_SCREEN_ROW)/2;
		

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view,glm::vec3(-Camx,-Camy,0.0f));
		glUniformMatrix4fv(view_loc,1,GL_FALSE,glm::value_ptr(view));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model,glm::vec3(tileWorldX,tileWorldY,0.0f));
		glUniformMatrix4fv(model_loc,1,GL_FALSE,glm::value_ptr(model));

		scale = glm::vec2(1.0f,1.0f);
		offset = glm::vec2(0.0f,0.0f);

		glUniform2f(scale_loc, scale.x, scale.y);
		glUniform2f(offset_loc, offset.x,offset.y);

		for(int row = 0; row < map.maxWorldRow; row++) 
		{
			for(int col = 0; col < map.maxWorldCol; col++) 
			{
				int tileID = map.mapTileNumber[row][col];
				
				if (tileID >= 0 && tileID < tile_manager.tiles.size()) 
				{
					GLuint currentTileTexture = tile_manager.tiles[tileID].texture_ID;
					
					float tileWorldX = col * TILE_SIZE;
					float tileWorldY = row * TILE_SIZE;
					
					glm::mat4 tile_mat = glm::mat4(1.0f);
					tile_mat = glm::translate(tile_mat, glm::vec3(tileWorldX, tileWorldY, 0.0f));
					glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(tile_mat));
					
					draw(shaderProgram, VAO1, currentTileTexture); 
				}
			}
		}
		
		glm::mat4 player_mat = glm::mat4(1.0f);
		player_mat = glm::translate(player_mat,glm::vec3(Player->attribx,Player->attriby,0.0f));
		glUniformMatrix4fv(model_loc,1,GL_FALSE,glm::value_ptr(player_mat));
		
		scale = glm::vec2(0.5f,0.5f);
		offset = glm::vec2(0.0f,0.0f);
		glUniform2f(scale_loc, scale.x, scale.y);
        glUniform2f(offset_loc, curr_off_x, curr_off_y);


		draw(shaderProgram,VAO1,curr_tex);
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (delta >= 1)
		{
			handleInput(window, Player,direction_ID);
			if(direction_ID==1){
                curr_tex=Up;
            }
            else if(direction_ID==2){
                curr_tex=Down;
            }
            else if(direction_ID==3){
                curr_tex=Left;
            }
            else if(direction_ID==4){
                curr_tex=Right;
            }

			if(drawCount%10==0){
                if (currID == 1) {
                    // 1 - Top Left
                    curr_off_x = 0.0f;
                    curr_off_y = 0.0f; 
                    currID++;
                }
                else if (currID == 2) {
                    // 2 - Top Right
                    curr_off_x = 0.5f;
                    curr_off_y = 0.0f;
                    currID++;
                }
                else if (currID == 3) {
                    // 3 - Bottom Left
                    curr_off_x = 0.0f;
                    curr_off_y = 0.5f;
                    currID++;
                }
                else if (currID == 4) {
                    // 4 - Bottom Right
                    curr_off_x = 0.5f;
                    curr_off_y = 0.5f;
                    currID = 1;
                }
            }
			drawCount++;
			delta--;
			
		}

		if (timer >= 1000000000)
		{
			std::cout << "FPS : " << drawCount << "\n";
			drawCount = 0;
			timer = 0;
		}
	}
}




void Load_Map(const char* path,Map& map){
	std::ifstream file(path);
	std::string line;

	if (!file.is_open()) 
    {
        std::cout << "Failed To Open File\n";
        return;
    }
	
	int row = 0;
	while (std::getline(file, line) && row<map.maxWorldRow) 
    {
        std::stringstream ss(line);
        int tile_ID;
		int col = 0;
        while (ss >> tile_ID && col<map.maxWorldCol) 
        {
            map.mapTileNumber[row][col] = tile_ID;
			col++;
        }
        row++;
    }
    file.close();
	std::cout<<"Map Loaded Successfully\n";
}

int main()
{	
	

	Entity* Player = new Entity(VELOCITY,VELOCITY);
	
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

	float spriteW = 48.0f / 2.0f;  // actual sprite width
    float spriteH = 48.0f / 2.0f; // actual sprite height

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
	floor.texture_ID = loadTexture("../resources/sprites/floor.png","Floor");
	wall.texture_ID = loadTexture("../resources/sprites/wall.png","Wall");

	Tile_Manager tile_manager;
	tile_manager.tiles.push_back(floor);
	tile_manager.tiles.push_back(wall);

	Map map;
	srand(time(NULL));
	static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> distr(1, 10);
    
    int ran_num = distr(gen);
	std::string map_path = "../resources/Map/map"+std::to_string(ran_num)+".txt";
	Load_Map(map_path.c_str(),map);
	std::cout<<"Map"<<ran_num<<'\n';

	GLuint Player_texture = loadTexture("../resources/sprites/spritesheet/up.png","up");
	std::cout << "Player Texture ID: " << Player_texture << "\n";
	if (Player_texture == 0) {
		std::cout << "CRITICAL ERROR: Player Texture failed to load!\n";
	}




	run(window, shaderProgram, VAO1, Player_texture,Player,map,tile_manager);

	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();
	glfwTerminate();
	delete(Player);
	return 0;
}
#include<iostream>
#include <glad/glad.h>
#include "../include/Map.h"
#include "../include/Tile_Manager.h"
#include "../include/ShaderClass.h"
#include "../include/VAO.h"
#include "../include/Tile_Manager.h"


Map::Map()
{
    mapTileNumber.assign(maxWorldRow,std::vector<int>(maxWorldCol,0));
}

void Map::map_generator(ShaderClass& shaderProgram, VAO& VAO1, GLuint modelUniID, Tile_Manager& tileManager, float cameraX, float cameraY, float windowWidth, float windowHeight){

}
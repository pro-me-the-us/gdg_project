#ifndef MAP_H
#define MAP_H

#include <vector>
#include <glad/glad.h>
#include "Tile_Manager.h"
class ShaderClass;
class VAO;
class Tile_Manager;

class Map
{
    public:
        const int maxWorldCol = 65;
        const int maxWorldRow = 65;
        std::vector<std::vector<int>> mapTileNumber;
        void map_generator(ShaderClass& shaderProgram, VAO& VAO1, GLuint modelUniID, Tile_Manager& tileManager, float cameraX, float cameraY, float windowWidth, float windowHeight);

        Map();
    
};

#endif
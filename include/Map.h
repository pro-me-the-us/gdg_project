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
        
        Map();
    
};

#endif
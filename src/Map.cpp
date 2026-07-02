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

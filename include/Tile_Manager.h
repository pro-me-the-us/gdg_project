#ifndef TILE_MANAGER_H
#define TILE_MANAGER_H


#include "Tile.h"
#include <vector>

class Tile_Manager
{   
    public:
        std::vector<Tile> tiles;
        Tile_Manager();
};

#endif
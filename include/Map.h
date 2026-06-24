#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Tile_Manager.h"

class Map
{
    public:
        const int maxWorldCol = 65;
        const int maxWorldRow = 65;
        std::vector<std::vector<int>> mapTileNumber;
        void map_generator(Tile_Manager& tile_manager);

        Map();
    
};

#endif
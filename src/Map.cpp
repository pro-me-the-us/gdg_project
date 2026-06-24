#include<iostream>
#include "../include/Map.h"
#include "../include/Tile_Manager.h"

Map::Map()
{
    mapTileNumber.assign(maxWorldRow,std::vector<int>(maxWorldCol,0));
}

void Map::map_generator(Tile_Manager& tile_manager){
    //doing nothing for now
}
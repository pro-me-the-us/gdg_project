#include<iostream>
#include "../include/Map.h"

Map::Map()
{
    map.assign(maxWorldRow,std::vector<int>(maxWorldCol,0));
}

void Map::map_generator(){
    //doing nothing for now
}
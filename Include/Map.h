#ifndef MAP_H
#define MAP_H

#include <vector>

class Map
{
    public:
        const int maxWorldCol = 65;
        const int maxWorldRow = 65;
        std::vector<std::vector<int>> map;
        void map_generator();

        Map();
    
};

#endif
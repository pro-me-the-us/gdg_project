#ifndef TILE_H
#define TILE_H


#include <fstream>

class Tile
{
    public:
        int ID;
        bool collision;
        unsigned int texture_ID;

        Tile(int ID);
};

#endif
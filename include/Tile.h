#ifndef TILE_H
#define TILE_H


#include <fstream>

class Tile
{
    public:
        int ID;
        bool collision;
        unsigned int texture_ID;
        bool bullet_destroy;

        Tile(int ID);
};

#endif
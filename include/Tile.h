#ifndef TILE_H
#define TILE_H


#include <fstream>

class Tile
{
    public:
        int ID;
        bool collision;
        std::fstream* file_ptr;    

        Tile();
        ~Tile();
};

#endif
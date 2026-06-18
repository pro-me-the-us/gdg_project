#ifndef TILE_H
#define TILE_H

#include <cstdint>
#include <fstream>

class Tile
{
    public:
        uint32_t ID;
        bool collision;
        std::fstream* file_ptr;    

        Tile();
        ~Tile();
};

#endif
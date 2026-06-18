#include<iostream>
#include <fstream>
#include "../include/Tile.h"

Tile::Tile()
{
    ID = 0;
    collision = false;
    file_ptr=nullptr;
}

Tile::~Tile()
{
    if(file_ptr != nullptr){
        if(file_ptr->is_open()) file_ptr->close();
        delete file_ptr;
    }
}


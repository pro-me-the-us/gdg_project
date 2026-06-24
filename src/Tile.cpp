#include<iostream>
#include <fstream>
#include "../include/Tile.h"

Tile::Tile(int ID)
{
    this->ID = ID;
    collision = false;
    texture_ID=0;
}


#include<iostream>
#include <fstream>
#include "../include/Tile.h"

Tile::Tile(int ID)
{
    this->ID = ID;
    this->collision = false;
    this->texture_ID=0;
    this->bullet_destroy = false;
}


#ifndef COLLISION_CHECKER_H
#define COLLISION_CHECKER_H
#include "../include/Entity.h"
#include "../include/Map.h"
#include "../include/Tile_Manager.h"

class CollisionChecker{
    public:
    bool CheckTile(Entity* Player,int direction,Map& map,Tile_Manager& tile_manager);
};

#endif
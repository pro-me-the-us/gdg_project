#include "../include/CollisionChecker.h"
#include "../include/Entity.h"
#include "../include/Map.h"
#include "../include/Tile_Manager.h"
#include <cmath>

bool CollisionChecker::CheckTile(Entity* player,int direction,Map& map,Tile_Manager& tile_manager){

    float leftWorldX   = player->attribx + player->HitBoxOffset.x;
    float rightWorldX  = leftWorldX + player->HitBoxSize.x;
    float bottomWorldY = player->attriby + player->HitBoxOffset.y;
    float topWorldY    = bottomWorldY + player->HitBoxSize.y;

    if (direction == 1) topWorldY += player->vely;   
    if (direction == 2) bottomWorldY -= player->vely;  
    if (direction == 3) leftWorldX -= player->velx;    
    if (direction == 4) rightWorldX += player->velx;     


    float shave = 1.0f;
    if (direction == 1 || direction == 2) 
    {
   
        leftWorldX += shave;
        rightWorldX -= shave;
    }
    else if (direction == 3 || direction == 4) 
    {
   
        bottomWorldY += shave;
        topWorldY -= shave;
    }


    int LeftCol   = static_cast<int>(std::floor(leftWorldX / 48.0f));
    int RightCol  = static_cast<int>(std::floor(rightWorldX / 48.0f));
    int TopRow    = static_cast<int>(std::floor(topWorldY / 48.0f));
    int BottomRow = static_cast<int>(std::floor(bottomWorldY / 48.0f));

  
    if (LeftCol < 0 || RightCol >= map.maxWorldCol || TopRow < 0 || BottomRow >= map.maxWorldRow){
        return true; 
    }

    int Tile1 = 0;
    int Tile2 = 0;


    if (direction == 1){ 
        Tile1 = map.mapTileNumber[TopRow][LeftCol];
        Tile2 = map.mapTileNumber[TopRow][RightCol];
    } 
    else if (direction == 2){ 
        Tile1 = map.mapTileNumber[BottomRow][LeftCol];
        Tile2 = map.mapTileNumber[BottomRow][RightCol];
    } 
    else if (direction == 3){ 
        Tile1 = map.mapTileNumber[TopRow][LeftCol];
        Tile2 = map.mapTileNumber[BottomRow][LeftCol];
    } 
    else if (direction == 4){ 
        Tile1 = map.mapTileNumber[TopRow][RightCol];
        Tile2 = map.mapTileNumber[BottomRow][RightCol];
    }

    
    if (Tile1 < 0 || Tile1 >= tile_manager.tiles.size() || Tile2 < 0 || Tile2 >= tile_manager.tiles.size()){
        return true; 
    }

    if (tile_manager.tiles[Tile1].collision || tile_manager.tiles[Tile2].collision){
        return true;
    }
    
    return false;
}
#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include "Bullet.h"
#include <vector>
#include "Map.h"
#include "Tile_Manager.h"
#include "ShaderClass.h"
#include "VAO.h"


class Bullet_Manager{
    public:
        unsigned int texture;
        std::vector<Bullet> Bullets;
        Bullet_Manager();
        void Create_Bullet(double Startx,double Starty,double targetx,double targety);
        void Update_Bullet(Map& map,Tile_Manager& tile_manager);
        void Draw_Bullet(Shader& shaderProgram,int model_loc,VAO& VAO1,int scale_loc,int offset_loc);
};

#endif
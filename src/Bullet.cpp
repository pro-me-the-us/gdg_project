#include "../include/Bullet.h"

Bullet::Bullet(float posx,float posy,float velx,float vely,float angle,float maxDistance,int ownerId){
    this->posx = posx;
    this->posy = posy;
    this->velx = velx;
    this->vely = vely;
    this->angle = angle;
    this->maxDistance = maxDistance;
    this->Distance_travelled = 0.0f;
    this->active = true;
    this->ownerId = ownerId;
}
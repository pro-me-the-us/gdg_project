#ifndef BULLET_H
#define BULLET_H

class Bullet{
    public:
    float posx;
    float posy;
    float velx;
    float vely;
    float angle;
    float maxDistance;
    float Distance_travelled;
    bool active;

    Bullet(float posx,float posy,float velx,float vely,float angle,float maxDistance);
};

#endif
#pragma once

#ifndef ENTITY_H
#define ENTIFY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Entity
{
    public:
        bool collisionON;
        float attribx;
        float attriby;
        float velx;
        float vely;
        glm::vec2 HitBoxOffset;
        glm::vec2 HitBoxSize;
        Entity(float velx , float vely,const char* type);
};


#endif
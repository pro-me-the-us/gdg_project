#include<iostream>
#include "../include/Entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Entity::Entity(float velx , float vely,const char* type)
{
    collisionON = false;
    attribx=0;
    attriby=0;
    this->velx=velx;
    this->vely=vely;
    
    if(type == "Player"){
        this->maxHealth=3;
        this->Health=3;
        HitBoxOffset = glm::vec2(-18.0f,-18.0f);
        HitBoxSize = glm::vec2(36.0f,36.0f);
    }
}

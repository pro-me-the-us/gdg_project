#include "../include/Bullet_Manager.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Bullet_Manager::Bullet_Manager(){
}

void Bullet_Manager::Create_Bullet(double Startx,double Starty,double targetx,double targety){
    double bulletX = targetx - Startx;
    double bulletY = targety - Starty;

    float theta = std::atan2(bulletY,bulletX);

    float total_distance = sqrt((bulletX*bulletX)+(bulletY*bulletY));

    float unitX = 0.0f;
    float unitY = 0.0f;

    if (total_distance > 0.0f) {
        unitX = bulletX / total_distance;
        unitY = bulletY / total_distance;
    }

    float bullet_speed = 5.0f;
    float maxDistance = 500.0f;

    float bulletVelX = unitX*bullet_speed;
    float bulletVelY = unitY*bullet_speed;

    Bullet new_bullet = Bullet(Startx,Starty,bulletVelX,bulletVelY,theta,maxDistance);

    this->Bullets.push_back(new_bullet);

    return;
}

void Bullet_Manager::Update_Bullet(Map& map,Tile_Manager& tile_manager){
    for(auto& bullet : Bullets){

        if(!bullet.active) continue;

        bullet.posx+=bullet.velx;
        bullet.posy+=bullet.vely;

        bullet.Distance_travelled += 5.0f;

        if(bullet.Distance_travelled>=bullet.maxDistance){
            bullet.active = false;
            continue;
        }

        int row = (int)std::floor(bullet.posy / 48.0f);
        int col = (int)std::floor(bullet.posx / 48.0f);

        if(row>=0 && row<map.maxWorldRow && col>=0 && col < map.maxWorldCol){
            int tileID = map.mapTileNumber[row][col];

            if (tileID >= 0 && tileID < (int)tile_manager.tiles.size()){
                if(tile_manager.tiles[tileID].bullet_destroy){
                    bullet.active=false;
                }
            }
        }
        else{
            bullet.active=false;
        }
    }

    Bullets.erase(
        std::remove_if(Bullets.begin(), Bullets.end(), [](const Bullet& b) {
            return !b.active;
        }), 
        Bullets.end()
    );
}

void Bullet_Manager::Draw_Bullet(Shader& shaderProgram,int model_loc,VAO& VAO1,int scale_loc,int offset_loc){
    shaderProgram.Activate();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "u_Texture"), 0);


    glm::vec2 scale = glm::vec2(0.5f,0.5f);
    glUniform2f(scale_loc,scale.x,scale.y);
    glm::vec2 offset = glm::vec2(0.0f,0.0f);
    glUniform2f(offset_loc, offset.x, offset.y);
    
    for(const auto& bullet:Bullets){
        if(!bullet.active) continue;

        glm::mat4 bullet_mat = glm::mat4(1.0f);
        bullet_mat = glm::translate(bullet_mat,glm::vec3(bullet.posx,bullet.posy,0.0f));
        bullet_mat = glm::rotate(bullet_mat,bullet.angle,glm::vec3(0.0f,0.0f,1.0f));
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(bullet_mat));

        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glUniform2f(scale_loc, 1.0f, 1.0f);
    glUniform2f(offset_loc, 0.0f, 0.0f);
}
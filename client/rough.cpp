#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main(){
    glm::vec4 vect(1.0f,0.0f,0.0f,1.0f);
    glm:: mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans,glm::vec3(3.0f,2.0f,1.0f));
    vect = trans*vect;
    std::cout<<vect.x<<" "<<vect.y<<" "<<vect.z;
}
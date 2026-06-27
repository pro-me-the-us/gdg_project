#version 330 core
layout (location = 0) in vec3 aPos;       
layout (location = 1) in vec2 aTexCoord;   

out vec2 TexCoord; 

uniform vec2 u_UVscale;
uniform vec2 u_UVoffset;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_Projection*u_Model*u_View*vec4(aPos, 1.0);
    TexCoord = (aTexCoord * u_UVscale) + u_UVoffset;
}
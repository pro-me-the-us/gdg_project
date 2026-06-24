#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 u_Pos;

void main()
{
    gl_Position = vec4(aPos.x + u_Pos.x, aPos.y + u_Pos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
}
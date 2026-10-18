#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_MVP;
uniform vec2 u_UVScale;
uniform vec2 u_UVOffset;

void main()
{
    gl_Position = u_MVP * vec4(aPos, 1.0);
    TexCoord = aTexCoord * u_UVScale + u_UVOffset;
}

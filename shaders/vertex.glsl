#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 vCol;
layout (location = 2) in vec2 uvCoord;

uniform mat4 MVP;
uniform mat4 VP;
out vec4 color;
out vec2 texCoord;

void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
    color = vCol;
    texCoord = uvCoord;
}
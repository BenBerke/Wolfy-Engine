#version 430 core

in vec4 outWallColor;
out vec4 FragColor;

void main() {
    FragColor = outWallColor;
}
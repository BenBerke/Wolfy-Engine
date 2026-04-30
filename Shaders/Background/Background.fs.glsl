#version 430 core

in vec2 vUV;

uniform sampler2D backgroundTexture;
uniform float playerAngle;

out vec4 FragColor;

void main() {
    vec2 uv = vUV;

    // Makes the background scroll horizontally when the player turns.
    uv.x += playerAngle / 360.0;

    FragColor = texture(backgroundTexture, uv);
}
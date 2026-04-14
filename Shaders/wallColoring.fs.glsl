#version 430 core

in vec4 vWallColor;
in vec2 vDepths;
noperspective in float vU;

out vec4 FragColor;

const float nearPlane = 0.01;
const float farPlane = 1000.0;

void main() {
    float invDepth = mix(1.0 / vDepths.x, 1.0 / vDepths.y, vU);
    float viewDepth = 1.0 / invDepth;

    float depth01 = clamp((viewDepth - nearPlane) / (farPlane - nearPlane), 0.0, 1.0);
    gl_FragDepth = depth01;

    FragColor = vWallColor;
}
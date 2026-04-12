#version 430 core

struct Wall {
    vec2 start;
    vec2 end;
};

layout(std430, binding = 0) readonly buffer WallBuffer {
    Wall walls[];
};

uniform vec2 playerPos;
uniform float playerAngle;

void main() {
    Wall w = walls[gl_VertexID];
    gl_Position = vec4(w.start + playerPos, 0.0, 1.0f + playerAngle);
    gl_PointSize = 20.0;
}
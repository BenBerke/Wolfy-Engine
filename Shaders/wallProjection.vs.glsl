#version 430 core

#define PI 3.14159265359

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 960

struct Wall {
    vec2 start, end;
    vec4 color;
};

layout(std430, binding = 0) readonly buffer WallBuffer {
    Wall walls[];
};

out vec4 outWallColor;

uniform vec2 playerPos;
uniform float playerAngle;

float degToRad(float angle){
    return angle * (PI / 180.0);
}

vec2 rotate(vec2 p, float angle){
    float c = cos(angle);
    float s = sin(angle);

    return vec2(
    p.x * c - p.y * s,
    p.x * s + p.y * c
    );
}

float FOV = 90;

float halfFov = FOV * 0.5;
float halfFovInRad = degToRad(halfFov);

float horizonY = SCREEN_HEIGHT * 0.5;
float wallHeight = 32;
float nearPlane = 0.01;

void main() {
    Wall wall = walls[gl_InstanceID];
    float playerAngleInRad = degToRad(playerAngle);

    vec2 relativeStart = wall.start - playerPos;
    vec2 relativeEnd = wall.end - playerPos;

    vec2 viewStart = rotate(relativeStart, playerAngleInRad);
    vec2 viewEnd   = rotate(relativeEnd, playerAngleInRad);

    if(viewStart.y <= nearPlane && viewEnd.y <= nearPlane){
        gl_Position = vec4(2.0, 2.0, .0, 1.0);
        return;
    }
    if (viewStart.y <= nearPlane) {
        float t = (nearPlane - viewStart.y) / (viewEnd.y - viewStart.y);
        viewStart = mix(viewStart, viewEnd, t);
    }

    if (viewEnd.y <= nearPlane) {
        float t = (nearPlane - viewEnd.y) / (viewStart.y - viewEnd.y);
        viewEnd = mix(viewEnd, viewStart, t);
    }
    float focalLength = (SCREEN_WIDTH * 0.5) / tan(halfFovInRad);

    float screenXStart = SCREEN_WIDTH * 0.5 + (viewStart.x / viewStart.y) * focalLength;
    float screenXEnd = SCREEN_WIDTH * 0.5f + (viewEnd.x / viewEnd.y) * focalLength;

    float projectedHeightStart = (wallHeight / viewStart.y) * focalLength;
    float projectedHeightEnd = (wallHeight / viewEnd.y) * focalLength;

    float topYStart = horizonY - projectedHeightStart * 0.5;
    float topYEnd = horizonY - projectedHeightEnd * 0.5;
    float bottomYStart = horizonY + projectedHeightStart * 0.5;
    float bottomYEnd = horizonY + projectedHeightEnd * 0.5;

    float leftX   = (screenXStart / SCREEN_WIDTH) * 2.0 - 1.0;
    float rightX  = (screenXEnd / SCREEN_WIDTH) * 2.0 - 1.0;

    float topLeftY = 1.0 - (topYStart / SCREEN_HEIGHT) * 2.0;
    float bottomLeftY = 1.0 - (bottomYStart / SCREEN_HEIGHT) * 2.0;
    float topRightY = 1.0 - (topYEnd / SCREEN_HEIGHT) * 2.0;
    float bottomRightY = 1.0 - (bottomYEnd / SCREEN_HEIGHT) * 2.0;

    vec2 verts[4] = vec2[4](
        vec2(leftX,  bottomLeftY),
        vec2(leftX, topLeftY),
        vec2(rightX, bottomRightY),
        vec2(rightX, topRightY)
    );

    outWallColor.xyzw = wall.color.xyzw / 255.0;
    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
}
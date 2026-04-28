//
// Created by berke on 4/27/2026.
//

#ifndef WOLFY_ENGINE_OBJECT_H
#define WOLFY_ENGINE_OBJECT_H
#include "Headers/Math/Vector/Vector2.hpp"

enum ObjectType {
    OBJ_PLAYER_SPAWN,
    OBJ_SPRITE,
    OBJ_DECAL,

    OBJ_COUNT,
};

struct Object {
    int id;
    ObjectType type;
    Vector2 position;
    int textureIndex;

    // For decals
    int wallIndex = -1;
};

#endif //WOLFY_ENGINE_OBJECT_H
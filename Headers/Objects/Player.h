//
// Created by berke on 4/13/2026.
//

#ifndef WOLFY_ENGINE_PLAYER_H
#define WOLFY_ENGINE_PLAYER_H

#include "../Math/Vector2.h"

namespace Player {
    inline Vector2 position = {0, 0};
    inline Vector2 velocity = {0, 0};
    inline float angle = 0;

    inline float speed = 15.0f;

    void Update();
}

#endif //WOLFY_ENGINE_PLAYER_H
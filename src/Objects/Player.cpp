//
// Created by berke on 4/13/2026.
//

#include "../../Headers/Objects/Player.h"
#include "../../Headers/Engine/InputManager.h"
#include "../../Headers/Engine/GameTime.h"

#define FRICTION .8f
#define TURN_SPEED 90.0f
#define SENSITIVITY .5f

namespace Player {
    void Update() {
        Vector2 input = {0.0f, 0.0f};
        if (InputManager::GetKey(SDL_SCANCODE_W)) input.y += 1.0f;
        if (InputManager::GetKey(SDL_SCANCODE_A)) input.x -= 1.0f;
        if (InputManager::GetKey(SDL_SCANCODE_S)) input.y -= 1.0f;
        if (InputManager::GetKey(SDL_SCANCODE_D)) input.x += 1.0f;

        if (InputManager::GetKey(SDL_SCANCODE_Q)) angle -= TURN_SPEED * GameTime::deltaTime;
        if (InputManager::GetKey(SDL_SCANCODE_E)) angle += TURN_SPEED * GameTime::deltaTime;

        angle += InputManager::GetMouseDelta().x * SENSITIVITY;

        float angleInRad = angle * M_PI / 180.0f;
        const float s = std::sin(angleInRad);
        const float c = std::cos(angleInRad);

        const Vector2 forward = {s, c};
        const Vector2 right = {c, -s};

        if (input.x != 0.0f || input.y != 0.0f) {
            const Vector2 moveDir = right * input.x + forward * input.y;
            velocity = moveDir.Normalized() * speed;
        } else {
            velocity *= FRICTION;
        }

        position += velocity * GameTime::deltaTime;
    }
}

#include "../../Headers/Engine/InputManager.h"
#include <algorithm>

namespace {
    const bool* keyboardState = nullptr;
    bool prevKeyboardState[SDL_SCANCODE_COUNT] = {};

    SDL_MouseButtonFlags mouseState = 0;
    SDL_MouseButtonFlags prevMouseState = 0;

    Vector2 mousePosition{};
    Vector2 mouseDelta{};
}

namespace InputManager {

    void BeginFrame() {
        if (!keyboardState) {
            SDL_PumpEvents();
            keyboardState = SDL_GetKeyboardState(nullptr);
        }

        std::copy_n(keyboardState, SDL_SCANCODE_COUNT, prevKeyboardState);

        SDL_PumpEvents();

        keyboardState = SDL_GetKeyboardState(nullptr);
        mouseState = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
        SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);
    }

    bool GetKeyDown(SDL_Scancode key) {
        return keyboardState[key] && !prevKeyboardState[key];
    }

    bool GetKey(SDL_Scancode key) {
        return keyboardState[key];
    }

    bool GetKeyUp(SDL_Scancode key) {
        return !keyboardState[key] && prevKeyboardState[key];
    }

    bool GetMouseButtonDown(Uint32 button) {
        return (mouseState & SDL_BUTTON_MASK(button)) &&
               !(prevMouseState & SDL_BUTTON_MASK(button));
    }

    bool GetMouseButton(Uint32 button) {
        return (mouseState & SDL_BUTTON_MASK(button)) != 0;
    }

    bool GetMouseButtonUp(Uint32 button) {
        return !(mouseState & SDL_BUTTON_MASK(button)) &&
                (prevMouseState & SDL_BUTTON_MASK(button));
    }

    Vector2 GetMousePosition() {
        return mousePosition;
    }

    Vector2 GetMouseDelta() {
        return mouseDelta;
    }
}
//
// Created by berke on 4/10/2026.
//
#ifndef WOLFY_ENGINE_RENDERER_H
#define WOLFY_ENGINE_RENDERER_H

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

#endif //WOLFY_ENGINE_RENDERER_H

namespace Renderer {
    inline SDL_Window* window;
    inline SDL_Renderer* renderer;
    inline SDL_GLContext glContext;

    bool Initialize();
    void Update();
}
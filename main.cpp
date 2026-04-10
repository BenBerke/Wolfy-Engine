#include <glad/glad.h>
#include <SDL3/SDL_video.h>

#include "Headers/Engine/InputManager.h"
#include "Headers/Engine/GameTime.h"
#include "Headers/Renderer/Renderer.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 600

int main() {
    Renderer::Initialize();
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        SDL_Log("Failed to initialize GLAD");
        return -1;
    }
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool running = true;
    while (running) {
        InputManager::BeginFrame();
        GameTime::Update();

        running = !InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE);

        Renderer::Update();
    }
}
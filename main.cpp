#include <filesystem>
#include <glad/glad.h>
#include <SDL3/SDL_video.h>

#include "Headers/Engine/InputManager.h"
#include "Headers/Engine/GameTime.h"

#include "Headers/Renderer/Renderer.h"
#include "Headers/Renderer/Shader.h"
#include "Headers/Renderer/MapEditor.h"

#include "Headers/Objects/Player.h"
#include "Headers/Objects/Wall.h"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 960


int main() {
    if (!Renderer::Initialize()) {
        SDL_Log("Failed to initialize Renderer: %s", SDL_GetError());
        return 1;
    }

    const Wall walls[] = {
        { { -0.8f, -0.8f }, { -0.4f, -0.2f } },
        { {  0.1f, -0.6f }, {  0.6f, -0.1f } },
        { { -0.2f,  0.3f }, {  0.7f,  0.8f } }
    };

    for (const Wall& wall : walls) MapEditor::AddWall(wall);

    if (!Renderer::CreateMap()) {
        SDL_Log("Failed to create map");
        return 1;   
    }

    bool running = true;
    while (running) {
        InputManager::BeginFrame();
        GameTime::Update();

        running = !InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE);

        if (InputManager::GetKeyDown(SDL_SCANCODE_1)) Player::position.x += 1;

        Renderer::Update(Player::position, Player::angle);

        SDL_GL_SwapWindow(Renderer::window);
    }

    Renderer::Destroy();
}
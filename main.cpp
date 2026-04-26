#include <format>

#include "Headers/Engine/GameTime.h"
#include "Headers/Engine/InputManager.h"

#include "Headers/Renderer/Renderer.h"
#include "Headers/Renderer/Shader.h"
#include "Headers/Renderer/MapEditor.h"

#include "Headers/Objects/Player.h"
#include "Headers/Objects/Wall.h"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 960


#include "../../Headers/Objects/Sector.h"

int main() {
    if (!Renderer::Initialize()) {
        SDL_Log("Failed to initialize Renderer: %s", SDL_GetError());
        return 1;
    }

    const Wall walls[] = {
        // =========================================================
        // Sector 0: Spawn room
        // floor = 0, ceiling = 40
        // Portals:
        // - north to Sector 1
        // - east  to Sector 3
        // - west  to Sector 4
        // =========================================================

        // South wall
        {{-120.0f, -100.0f}, {120.0f, -100.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},

        // North wall split around corridor portal
        {{120.0f, 100.0f}, {30.0f, 100.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},
        {{-30.0f, 100.0f}, {-120.0f, 100.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},

        // East wall split around raised-room portal
        {{120.0f, -100.0f}, {120.0f, -20.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},
        {{120.0f, 50.0f}, {120.0f, 100.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},

        // West wall split around sunken-room portal
        {{-120.0f, 100.0f}, {-120.0f, 40.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},
        {{-120.0f, -40.0f}, {-120.0f, -100.0f}, {180.0f, 180.0f, 180.0f, 255.0f}, 0, -1},

        // Portal: Spawn <-> Corridor
        // Direction east, left side is north => front = 1, back = 0
        {{-30.0f, 100.0f}, {30.0f, 100.0f}, {255.0f, 40.0f, 40.0f, 255.0f}, 1, 0},

        // Portal: Spawn <-> East raised room
        // Direction south, left side is east => front = 3, back = 0
        {{120.0f, 50.0f}, {120.0f, -20.0f}, {255.0f, 40.0f, 40.0f, 255.0f}, 3, 0},

        // Portal: Spawn <-> West sunken room
        // Direction north, left side is west => front = 4, back = 0
        {{-120.0f, -40.0f}, {-120.0f, 40.0f}, {255.0f, 40.0f, 40.0f, 255.0f}, 4, 0},


        // =========================================================
        // Sector 1: North corridor
        // floor = 0, ceiling = 36
        // Portals:
        // - south to Sector 0
        // - north to Sector 2
        // =========================================================

        // Left wall
        {{-30.0f, 260.0f}, {-30.0f, 100.0f}, {130.0f, 160.0f, 220.0f, 255.0f}, 1, -1},

        // Right wall
        {{30.0f, 100.0f}, {30.0f, 260.0f}, {130.0f, 160.0f, 220.0f, 255.0f}, 1, -1},

        // Portal: Corridor <-> Central hall
        // Direction east, left side is north => front = 2, back = 1
        {{-30.0f, 260.0f}, {30.0f, 260.0f}, {255.0f, 40.0f, 40.0f, 255.0f}, 2, 1},


        // =========================================================
        // Sector 2: Central hall
        // floor = 4, ceiling = 52
        // Portals:
        // - south to Sector 1
        // - east  to Sector 5
        // =========================================================

        // South wall split around corridor portal
        {{-180.0f, 260.0f}, {-30.0f, 260.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},
        {{30.0f, 260.0f}, {180.0f, 260.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},

        // East wall split around upper-room portal
        {{180.0f, 260.0f}, {180.0f, 320.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},
        {{180.0f, 390.0f}, {180.0f, 480.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},

        // North wall
        {{180.0f, 480.0f}, {-180.0f, 480.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},

        // West wall
        {{-180.0f, 480.0f}, {-180.0f, 260.0f}, {200.0f, 140.0f, 100.0f, 255.0f}, 2, -1},

        // Portal: Central hall <-> Upper east room
        // Direction south, left side is east => front = 5, back = 2
        {{180.0f, 390.0f}, {180.0f, 320.0f}, {255.0f, 40.0f, 40.0f, 255.0f}, 5, 2},


        // =========================================================
        // Sector 3: East raised room
        // floor = 8, ceiling = 56
        // Portal:
        // - west to Sector 0
        // =========================================================

        // South wall
        {{120.0f, -20.0f}, {280.0f, -20.0f}, {100.0f, 220.0f, 130.0f, 255.0f}, 3, -1},

        // East wall
        {{280.0f, -20.0f}, {280.0f, 140.0f}, {100.0f, 220.0f, 130.0f, 255.0f}, 3, -1},

        // North wall
        {{280.0f, 140.0f}, {120.0f, 140.0f}, {100.0f, 220.0f, 130.0f, 255.0f}, 3, -1},

        // West wall above portal
        {{120.0f, 140.0f}, {120.0f, 50.0f}, {100.0f, 220.0f, 130.0f, 255.0f}, 3, -1},


        // =========================================================
        // Sector 4: West sunken room
        // floor = -4, ceiling = 36
        // Portal:
        // - east to Sector 0
        // =========================================================

        // South wall
        {{-280.0f, -40.0f}, {-120.0f, -40.0f}, {120.0f, 120.0f, 200.0f, 255.0f}, 4, -1},

        // East wall above portal
        {{-120.0f, 40.0f}, {-120.0f, 120.0f}, {120.0f, 120.0f, 200.0f, 255.0f}, 4, -1},

        // North wall
        {{-120.0f, 120.0f}, {-280.0f, 120.0f}, {120.0f, 120.0f, 200.0f, 255.0f}, 4, -1},

        // West wall
        {{-280.0f, 120.0f}, {-280.0f, -40.0f}, {120.0f, 120.0f, 200.0f, 255.0f}, 4, -1},


        // =========================================================
        // Sector 5: Upper east room
        // floor = 8, ceiling = 60
        // Portal:
        // - west to Sector 2
        // =========================================================

        // South wall
        {{180.0f, 300.0f}, {340.0f, 300.0f}, {220.0f, 180.0f, 110.0f, 255.0f}, 5, -1},

        // East wall
        {{340.0f, 300.0f}, {340.0f, 460.0f}, {220.0f, 180.0f, 110.0f, 255.0f}, 5, -1},

        // North wall
        {{340.0f, 460.0f}, {180.0f, 460.0f}, {220.0f, 180.0f, 110.0f, 255.0f}, 5, -1},

        // West wall split around hall portal
        {{180.0f, 320.0f}, {180.0f, 300.0f}, {220.0f, 180.0f, 110.0f, 255.0f}, 5, -1},
        {{180.0f, 460.0f}, {180.0f, 390.0f}, {220.0f, 180.0f, 110.0f, 255.0f}, 5, -1}
    };

    const Sector sectors[] = {
        // =========================================================
        // Sector 0: Spawn room
        // =========================================================
        {
            {
                {-120.0f, -100.0f},
                {120.0f, -100.0f},
                {120.0f, 100.0f},
                {-120.0f, 100.0f}
            },
            {},
            40.0f,
            0.0f,
            {60.0f, 70.0f, 95.0f},
            {95.0f, 95.0f, 95.0f}
        },

        // =========================================================
        // Sector 1: North corridor
        // =========================================================
        {
            {
                {-30.0f, 100.0f},
                {30.0f, 100.0f},
                {30.0f, 260.0f},
                {-30.0f, 260.0f}
            },
            {},
            36.0f,
            0.0f,
            {85.0f, 80.0f, 55.0f},
            {110.0f, 100.0f, 70.0f}
        },

        // =========================================================
        // Sector 2: Central hall
        // =========================================================
        {
            {
                {-180.0f, 260.0f},
                {180.0f, 260.0f},
                {180.0f, 480.0f},
                {-180.0f, 480.0f}
            },
            {},
            52.0f,
            4.0f,
            {70.0f, 55.0f, 55.0f},
            {120.0f, 85.0f, 70.0f}
        },

        // =========================================================
        // Sector 3: East raised room
        // =========================================================
        {
            {
                {120.0f, -20.0f},
                {280.0f, -20.0f},
                {280.0f, 140.0f},
                {120.0f, 140.0f}
            },
            {},
            56.0f,
            8.0f,
            {50.0f, 90.0f, 65.0f},
            {85.0f, 140.0f, 95.0f}
        },

        // =========================================================
        // Sector 4: West sunken room
        // =========================================================
        {
            {
                {-280.0f, -40.0f},
                {-120.0f, -40.0f},
                {-120.0f, 120.0f},
                {-280.0f, 120.0f}
            },
            {},
            36.0f,
            -4.0f,
            {65.0f, 75.0f, 105.0f},
            {60.0f, 70.0f, 115.0f}
        },

        // =========================================================
        // Sector 5: Upper east room
        // =========================================================
        {
            {
                {180.0f, 300.0f},
                {340.0f, 300.0f},
                {340.0f, 460.0f},
                {180.0f, 460.0f}
            },
            {},
            60.0f,
            8.0f,
            {105.0f, 75.0f, 45.0f},
            {135.0f, 105.0f, 70.0f}
        }
    };

    for (const Wall &wall: walls) MapEditor::AddWall(wall);
    for (const Sector &sector: sectors) MapEditor::AddSector(sector);

    Player::FindCurrentSector(MapEditor::sectors);

    if (!Renderer::CreateMap()) {
        SDL_Log("Failed to create map");
        return 1;
    }

    static float timer = 0;
    static float timerHelper = 0;

    static int fps = 0;

    bool running = true;
    while (running) {
        timer = GameTime::time;
        InputManager::BeginFrame();
        GameTime::Update();
        Player::Update(MapEditor::walls, MapEditor::sectors);


        running = !InputManager::GetKeyDown(SDL_SCANCODE_ESCAPE);

        if (InputManager::GetKeyDown(SDL_SCANCODE_1)) Player::position.x += 1;

        Renderer::Update(Player::position, Player::angle);
        Renderer::RenderTextRaw(std::format("FPS: {}", fps), 0, 0, 0.5f, Vector3{255, 255, 255});

        SDL_GL_SwapWindow(Renderer::window);

        if (timer > timerHelper + 1.3f) {
            fps = static_cast<int>(GameTime::GetFPS());
            timerHelper = timer;
        }
    }

    Renderer::Destroy();
}

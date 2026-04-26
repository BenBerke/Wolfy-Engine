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
#include "../../Headers/Renderer/TextureManager.h"

int main() {
    if (!Renderer::Initialize()) {
        SDL_Log("Failed to initialize Renderer: %s", SDL_GetError());
        return 1;
    }
    int brickTexture = TextureManager::CreateTexture("../Assets/Textures/wall.png");
    int woodTexture = TextureManager::CreateTexture("../Assets/Textures/wood.png");
    int whiteTexture = TextureManager::CreateTexture("../Assets/Textures/white.png");
    int floorTexture = TextureManager::CreateTexture("../Assets/Textures/floor.png");
    int humanTexture = TextureManager::CreateTexture("../Assets/Textures/human.png");
    if (brickTexture == -1 || woodTexture == -1 || whiteTexture == -1) {
        SDL_Log("Failed to load one or more textures");
        return 1;
    }

    // region Map

const Vector4 WHITE = {255.0f, 255.0f, 255.0f, 255.0f};
const Vector4 PORTAL_RED = {255.0f, 80.0f, 80.0f, 255.0f};

// =========================================================
// Sector 0: Spawn hex room
// =========================================================
constexpr Vector2 P0 = {-80.0f, -60.0f};
constexpr Vector2 P1 = { 40.0f, -80.0f};
constexpr Vector2 P2 = {100.0f, -20.0f};
constexpr Vector2 P3 = { 90.0f,  60.0f};
constexpr Vector2 P4 = { 10.0f,  90.0f};
constexpr Vector2 P5 = {-90.0f,  40.0f};

// =========================================================
// Sector 1: Angled connector
// =========================================================
constexpr Vector2 C1 = {170.0f, -10.0f};
constexpr Vector2 C2 = {160.0f,  70.0f};

// =========================================================
// Sector 2: Main irregular hall
// =========================================================
constexpr Vector2 M0 = {240.0f, -40.0f};
constexpr Vector2 M1 = {320.0f,  20.0f};
constexpr Vector2 M2 = {340.0f, 100.0f};
constexpr Vector2 M3 = {280.0f, 170.0f};
constexpr Vector2 M4 = {190.0f, 160.0f};

// =========================================================
// Sector 3: Raised side room
// =========================================================
constexpr Vector2 R0 = {430.0f,  80.0f};
constexpr Vector2 R1 = {440.0f, 170.0f};
constexpr Vector2 R2 = {350.0f, 210.0f};

// =========================================================
// Sector 4: Lower cave room
// =========================================================
constexpr Vector2 L0 = {120.0f, 220.0f};
constexpr Vector2 L1 = { 60.0f, 160.0f};
constexpr Vector2 L2 = { 80.0f,  80.0f};

// =========================================================
// Sector 5: Stair sector
// =========================================================
constexpr Vector2 S0 = {300.0f, 230.0f};
constexpr Vector2 S1 = {230.0f, 260.0f};
constexpr Vector2 S2 = {170.0f, 220.0f};

// =========================================================
// Sector 6: Upper platform
// =========================================================
constexpr Vector2 U0 = {360.0f, 270.0f};
constexpr Vector2 U1 = {300.0f, 330.0f};
constexpr Vector2 U2 = {210.0f, 310.0f};

const Wall walls[] = {
    // =========================================================
    // Sector 0: Spawn room
    // =========================================================

    {P0, P1, WHITE,      0, -1, brickTexture},
    {P1, P2, WHITE,      0, -1, brickTexture},
    {P2, P3, PORTAL_RED, 0,  1, brickTexture},
    {P3, P4, WHITE,      0, -1, brickTexture},
    {P4, P5, WHITE,      0, -1, brickTexture},
    {P5, P0, WHITE,      0, -1, brickTexture},

    // =========================================================
    // Sector 1: Angled connector
    // =========================================================

    {P2, C1, WHITE,      1, -1, brickTexture},
    {C1, C2, PORTAL_RED, 1,  2, brickTexture},
    {C2, P3, WHITE,      1, -1, brickTexture},

    // =========================================================
    // Sector 2: Main irregular hall
    // =========================================================

    {C1, M0, WHITE,      2, -1, brickTexture},
    {M0, M1, WHITE,      2, -1, brickTexture},
    {M1, M2, WHITE,      2, -1, brickTexture},
    {M2, M3, PORTAL_RED, 2,  3, brickTexture},
    {M3, M4, PORTAL_RED, 2,  5, brickTexture},
    {M4, C2, PORTAL_RED, 2,  4, brickTexture},

    // =========================================================
    // Sector 3: Raised side room
    // =========================================================

    {M2, R0, WHITE, 3, -1, brickTexture},
    {R0, R1, WHITE, 3, -1, brickTexture},
    {R1, R2, WHITE, 3, -1, brickTexture},
    {R2, M3, WHITE, 3, -1, brickTexture},

    // =========================================================
    // Sector 4: Lower cave room
    // =========================================================

    {M4, L0, WHITE, 4, -1, brickTexture},
    {L0, L1, WHITE, 4, -1, brickTexture},
    {L1, L2, WHITE, 4, -1, brickTexture},
    {L2, C2, WHITE, 4, -1, brickTexture},

    // =========================================================
    // Sector 5: Stair sector
    // =========================================================

    {M3, S0, WHITE,      5, -1, brickTexture},
    {S0, S1, PORTAL_RED, 5,  6, brickTexture},
    {S1, S2, WHITE,      5, -1, brickTexture},
    {S2, M4, WHITE,      5, -1, brickTexture},

    // =========================================================
    // Sector 6: Upper platform
    // =========================================================

    {S0, U0, WHITE, 6, -1, brickTexture},
    {U0, U1, WHITE, 6, -1, brickTexture},
    {U1, U2, WHITE, 6, -1, brickTexture},
    {U2, S1, WHITE, 6, -1, brickTexture},
};

const Sector sectors[] = {
    // =========================================================
    // Sector 0: Spawn room
    // =========================================================
    {
        {P0, P1, P2, P3, P4, P5},
        {},
        40.0f, // ceilingHeight
        0.0f,  // floorHeight
        {255.0f, 255.0f, 255.0f}, // ceilingColor
        {255.0f, 255.0f, 255.0f}, // floorColor
        woodTexture,              // ceiling texture
        floorTexture              // floor texture
    },

    // =========================================================
    // Sector 1: Angled connector
    // =========================================================
    {
        {P2, C1, C2, P3},
        {},
        36.0f,
        0.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    },

    // =========================================================
    // Sector 2: Main irregular hall
    // =========================================================
    {
        {C1, M0, M1, M2, M3, M4, C2},
        {},
        48.0f,
        4.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    },

    // =========================================================
    // Sector 3: Raised side room
    // =========================================================
    {
        {M2, R0, R1, R2, M3},
        {},
        60.0f,
        12.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    },

    // =========================================================
    // Sector 4: Lower cave room
    // =========================================================
    {
        {C2, M4, L0, L1, L2},
        {},
        34.0f,
        -4.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    },

    // =========================================================
    // Sector 5: Stair sector
    // =========================================================
    {
        {M4, M3, S0, S1, S2},
        {},
        52.0f,
        8.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    },

    // =========================================================
    // Sector 6: Upper platform
    // =========================================================
    {
        {S0, U0, U1, U2, S1},
        {},
        64.0f,
        16.0f,
        {255.0f, 255.0f, 255.0f},
        {255.0f, 255.0f, 255.0f},
        woodTexture,
        floorTexture
    }
};

// endregion

    for (const Wall& wall : walls) {
        MapEditor::AddWall(wall);
    }

    for (const Sector& sector : sectors) {
        MapEditor::AddSector(sector);
    }

    Player::position = {-30.0f, 0.0f};
    Player::FindCurrentSector(MapEditor::sectors);

    if (Player::currentSector == -1) {
        SDL_Log("Player is not inside any sector");
        return 1;
    }

    if (!Renderer::CreateMap()) {
        SDL_Log("Failed to create map");
        return 1;
    }
    // endregion

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

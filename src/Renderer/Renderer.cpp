//
// Created by berke on 4/10/2026.
//

#include <glad/glad.h>
#include "../../Headers/Renderer/Renderer.h"

#include <map>
#include <memory>
#include <SDL3/SDL_init.h>

#include "../../Headers/Objects/Wall.h"
#include "../../Headers/Renderer/MapEditor.h"

#include "../../Headers/Math/Matrix/Matrix4.h"
#include "Headers/Engine/InputManager.h"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 960

#define DEBUG true

constexpr Uint64 windowFlags = SDL_WINDOW_OPENGL;

std::unique_ptr<Shader> projectionShader;
static GLint playerPosUniform;
static GLint playerAngleUniform;

std::unique_ptr<Shader> debugShader;

std::unique_ptr<Shader> textShader;
static FT_Library ft;
static FT_Face face;

namespace {
    struct Character {
        unsigned int textureID;  // ID handle of the glyph texture
        Vector2   Size;       // Size of glyph
        Vector2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    std::map<char, Character> Characters;
}

namespace {
    constexpr float DEBUG_MAP_SCALE = 200.0f;
    constexpr float DEBUG_PLAYER_HALF_SIZE = 0.01f;
    constexpr float DEBUG_FOV_DEG = 90.0f;
    constexpr float DEBUG_FOV_LINE_LENGTH = 100.0f;

    float DegToRad(const float degrees) {
        return degrees * 3.14159265359f / 180.0f;
    }

    Vector2 RotatePoint(const Vector2& p, const float angleRad) {
        const float c = std::cos(angleRad);
        const float s = std::sin(angleRad);

        return {
            p.x * c - p.y * s,
            p.x * s + p.y * c
        };
    }

    Vector2 WorldToDebugNdc(const Vector2& worldPoint, const Vector2& playerPos) {
        const Vector2 relative = {
            worldPoint.x - playerPos.x,
            worldPoint.y - playerPos.y
        };

        return {
            relative.x / DEBUG_MAP_SCALE,
            relative.y / DEBUG_MAP_SCALE
        };
    }
}

namespace Renderer {
    // region Initialization
    bool InitializeOpenGL() {
        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)) {
            SDL_Log("SDL_GL_SetAttribute Major Error: %s\n", SDL_GetError());
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
            SDL_Log("SDL_GL_SetAttribute Minor Error: %s\n", SDL_GetError());
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
            SDL_Log("SDL_GL_SetAttribute Core Error: %s\n", SDL_GetError());
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
            SDL_Log("SDL_GL_SetAttribute Double Error: %s\n", SDL_GetError());
            return false;
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24)) {
            SDL_Log("SDL_GL_SetAttribute Depth Error: %s\n", SDL_GetError());
            return false;
        }

        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            SDL_Log("SDL_GL_CreateContext Error: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            window = nullptr;
            return false;
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            SDL_Log("Failed to initialize GLAD");
            return false;
        }
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (!SDL_GL_SetSwapInterval(1)) {
            SDL_Log("SDL_GL_SetSwapInterval Warning: %s", SDL_GetError());
        }
        return true;
    }

    bool InitializeFont() {
        if (FT_Init_FreeType(&ft)) {
            SDL_Log("FT_Init_FreeType Error");
            return false;
        }
        if (FT_New_Face(ft, "../Assets/Fonts/arial.ttf", 0, &face)) {
            SDL_Log("FT_New_Face Error");
            return false;
        }
        FT_Set_Pixel_Sizes(face, 0, 48);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                SDL_Log("Failed to load glyph: %c", c);
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {
                texture,
                Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x),
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    bool Initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
            return false;
        }

        window = SDL_CreateWindow("Wolfy Engine", SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
        if (!window) {
            SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
            return false;
        }

        if (!InitializeOpenGL()) {
            SDL_Log("Initialize OpenGL Error: %s\n", SDL_GetError());
            return false;
        }

        // --- WALL / PROJECTION SHADER ---
        projectionShader = std::make_unique<Shader>(
            "../Shaders/wallProjection.vs.glsl",
            "../Shaders/wallColoring.fs.glsl"
        );
        if (projectionShader->ID == 0) {
            SDL_Log("Projection Shader creation failed");
            return false;
        }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindVertexArray(0);

        // --- DEBUG SHADER + DEBUG VAO/VBO ---
        if (DEBUG) {
            debugShader = std::make_unique<Shader>(
                "../Shaders/debug.vs.glsl",
                "../Shaders/debug.fs.glsl"
            );
            if (debugShader->ID == 0) {
                SDL_Log("Debug Shader creation failed");
                return false;
            }

            glGenVertexArrays(1, &debugVAO);
            glGenBuffers(1, &debugVBO);

            glBindVertexArray(debugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, nullptr, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            debugColorUniform = glGetUniformLocation(debugShader->ID, "uColor");
            if (debugColorUniform == -1) {
                SDL_Log("Failed to get debug shader uniform location uColor");
                return false;
            }
        }

        // --- TEXT SHADER ---
        textShader = std::make_unique<Shader>(
            "../Shaders/glyph.vs.glsl",
            "../Shaders/glyph.fs.glsl"
        );
        if (textShader->ID == 0) {
            SDL_Log("Text Shader creation failed");
            return false;
        }

        textShader->use();

        Matrix4 projection = Matrix4::Orthographic(
            0.0f, static_cast<float>(SCREEN_WIDTH),
            0.0f, static_cast<float>(SCREEN_HEIGHT),
            -1.0f, 1.0f
        );

        glUniformMatrix4fv(
            glGetUniformLocation(textShader->ID, "projection"),
            1,
            GL_TRUE,
            &projection.m[0][0]
        );

        glUniform1i(glGetUniformLocation(textShader->ID, "text"), 0);

        if (!InitializeFont()) {
            SDL_Log("Failed to initialize font");
            return false;
        }

        SDL_SetWindowRelativeMouseMode(window, true);
        return true;
    }

    // endregion

    void RenderText(const Shader &s, const std::string& text, float x, const float y, const float scale, const Vector3 color)
    {
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(textVAO);
        glActiveTexture(GL_TEXTURE0);
        s.use();
        glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        for (char c : text)
        {
            auto [textureID, Size, Bearing, Advance] = Characters[c];

            const float xPos = x + Bearing.x * scale;
            const float yPos = y - (Size.y - Bearing.y) * scale;

            const float w = Size.x * scale;
            const float h = Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xPos,     yPos + h,   0.0f, 0.0f },
                { xPos,     yPos,       0.0f, 1.0f },
                { xPos + w, yPos,       1.0f, 1.0f },

                { xPos,     yPos + h,   0.0f, 0.0f },
                { xPos + w, yPos,       1.0f, 1.0f },
                { xPos + w, yPos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, textureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderTextRaw(const std::string& text, const float x, const float y, const float scale, const Vector3 color) {
        RenderText(*textShader, text, x, y, scale, color);
    }

    bool CreateMap() {
        glGenBuffers(1, &wallSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, wallSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MapEditor::walls.size() * sizeof(Wall), MapEditor::walls.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, wallSSBO);
        glEnable(GL_PROGRAM_POINT_SIZE);

        // -- UNIFORMS--
        playerPosUniform = glGetUniformLocation(projectionShader->ID, "playerPos");
        if (playerPosUniform == -1) {
            SDL_Log("Failed to get shader uniform location playerPos");
            return false;
        }

        playerAngleUniform = glGetUniformLocation(projectionShader->ID, "playerAngle");
        if (playerAngleUniform == -1) {
            SDL_Log("Failed to get shader uniform location playerAngle");
            return false;
        }
        return true;
    }

    // region Debug Functions
    void DrawDebugLine(const Vector2 start, const Vector2 end) {
        const float verts[] = {
            start.x, start.y,
            end.x,   end.y
        };

        debugShader->use();
        glUniform4f(debugColorUniform, 1.0f, 1.0f, 1.0f, 1.0f);

        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_LINES, 0, 2);
    }

    void DrawDebugRect(const Vector2 pos, const float sizeX, const float sizeY) {
        const float verts[] = {
            pos.x - sizeX, pos.y - sizeY, // bottom-left
            pos.x + sizeX, pos.y - sizeY, // bottom-right
            pos.x + sizeX, pos.y + sizeY, // top-right
            pos.x - sizeX, pos.y + sizeY  // top-left
        };

        debugShader->use();
        glUniform4f(debugColorUniform, 1.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }
    // endregion

    void Update(const Vector2& playerPos, const float playerAngle) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(.2f, .3f, .3f, 1.0f);

        projectionShader->use();
        glBindVertexArray(VAO);

        glUniform2f(playerPosUniform, playerPos.x, playerPos.y);
        glUniform1f(playerAngleUniform, playerAngle);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(MapEditor::walls.size()));

        if (!InputManager::GetKey(SDL_SCANCODE_TAB)) return;
        glDisable(GL_DEPTH_TEST);

        DrawDebugRect({0.0f, 0.0f}, DEBUG_PLAYER_HALF_SIZE, DEBUG_PLAYER_HALF_SIZE);

        for (const Wall& wall : MapEditor::walls) {
            const Vector2 start = WorldToDebugNdc(wall.start, playerPos);
            const Vector2 end = WorldToDebugNdc(wall.end, playerPos);

            DrawDebugLine(start, end);
        }

        const float halfFovRad = DegToRad(DEBUG_FOV_DEG * 0.5f);
        const float angleRad = DegToRad(playerAngle);

        const Vector2 baseForward = {0.0f, DEBUG_FOV_LINE_LENGTH};

        Vector2 leftFov = RotatePoint(baseForward, angleRad - halfFovRad);
        Vector2 rightFov = RotatePoint(baseForward, angleRad + halfFovRad);

        leftFov.x *= -1;
        rightFov.x *= -1;

        DrawDebugLine({0.0f, 0.0f}, leftFov);
        DrawDebugLine({0.0f, 0.0f}, rightFov);
    }

    void Destroy() {
        glDeleteBuffers(1, &wallSSBO);
        glDeleteVertexArrays(1, &VAO);
        if (glContext) {
            SDL_GL_DestroyContext(glContext);
            glContext = nullptr;
        }

        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }
}
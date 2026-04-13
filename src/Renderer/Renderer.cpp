//
// Created by berke on 4/10/2026.
//

#include <glad/glad.h>
#include "../../Headers/Renderer/Renderer.h"

#include <memory>
#include <SDL3/SDL_init.h>

#include "../../Headers/Objects/Wall.h"
#include "../../Headers/Renderer/MapEditor.h"

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 960

constexpr Uint64 windowFlags = SDL_WINDOW_OPENGL;

std::unique_ptr<Shader> shader;
static GLint playerPosUniform;
static GLint playerAngleUniform;

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

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        shader = std::make_unique<Shader>("../Shaders/vertex.vs.glsl", "../Shaders/frag.fs.glsl");
        if (shader->ID == 0) {
            SDL_Log("Shader creation failed");
            return false;
        }

        SDL_SetWindowRelativeMouseMode(window, true);
        return true;
    }

    // endregion

    bool CreateMap() {
        glGenBuffers(1, &wallSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, wallSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MapEditor::walls.size() * sizeof(Wall), MapEditor::walls.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, wallSSBO);
        glEnable(GL_PROGRAM_POINT_SIZE);

        // -- UNIFORMS--
        playerPosUniform = glGetUniformLocation(shader->ID, "playerPos");
        if (playerPosUniform == -1) {
            SDL_Log("Failed to get shader uniform location playerPos");
            return false;
        }

        playerAngleUniform = glGetUniformLocation(shader->ID, "playerAngle");
        if (playerAngleUniform == -1) {
            SDL_Log("Failed to get shader uniform location playerAngle");
            return false;
        }
        return true;
    }

    void Update(const Vector2& playerPos, const float playerAngle) {
        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();
        glBindVertexArray(VAO);

        glUniform2f(playerPosUniform, playerPos.x, playerPos.y);
        glUniform1f(playerAngleUniform, playerAngle);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(MapEditor::walls.size()));
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
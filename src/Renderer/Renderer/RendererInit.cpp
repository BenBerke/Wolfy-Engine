#include "../../Headers/Renderer/Renderer/Renderer.hpp"
#include "RendererInternal.hpp"
#include "Headers/Project/ProjectManager.hpp"

#include "Headers/Math/Matrix/Matrix4.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3_image/SDL_image.h>

namespace RendererInternal {
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

        Renderer::glContext = SDL_GL_CreateContext(Renderer::window);
        if (!Renderer::glContext) {
            SDL_Log("SDL_GL_CreateContext Error: %s", SDL_GetError());
            SDL_DestroyWindow(Renderer::window);
            Renderer::window = nullptr;
            return false;
        }

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            SDL_Log("Failed to initialize GLAD");
            return false;
        }

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (!SDL_GL_SetSwapInterval(0)) {
            SDL_Log("SDL_GL_SetSwapInterval Warning: %s", SDL_GetError());
        }

        return true;
    }
}

namespace Renderer {
    using namespace RendererInternal;
    //region init
    bool InitSDL() {
        if (SDL_Init(SDL_INIT_VIDEO) == false) {
            SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
            return false;
        }

        window = SDL_CreateWindow("Wolfy Engine", SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS);
        if (!window) {
            SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
            return false;
        }

        const fs::path iconPath = ProjectManager::GetEngineBasePath() / "LauncherAssets" / "Fox.png";
        SDL_Surface* windowIcon = IMG_Load(iconPath.string().c_str());

        if (windowIcon == nullptr)
            SDL_Log("RendererInit.cpp failed to load window icon: %s", SDL_GetError());
        else {
            if (!SDL_SetWindowIcon(window, windowIcon)) {
                SDL_Log("RendeerInit.cpp failed to set window icon: %s", SDL_GetError());
            }
            SDL_DestroySurface(windowIcon);
        }

        if (!InitializeOpenGL()) {
            SDL_Log("Initialize OpenGL Error: %s\n", SDL_GetError());
            return false;
        }
        return true;
    }
    bool InitImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(window, glContext);
        ImGui_ImplOpenGL3_Init("#version 430");

        return true;
    }

    bool InitProjection() {
        const fs::path shaderPath = ProjectManager::GetEngineBasePath() / "Shaders";
        projectionShader = std::make_unique<Shader>(
    (shaderPath / "Rendering" / "Rendering.vs.glsl").string().c_str(),
    (shaderPath / "Rendering" / "Rendering.fs.glsl").string().c_str()
        );

        if (projectionShader->ID == 0) {
            SDL_Log("Projection Shader creation failed");
            return false;
        }

        backgroundShader = std::make_unique<Shader>(
    (shaderPath / "Background" / "Background.vs.glsl").string().c_str(),
    (shaderPath / "Background" / "Background.fs.glsl").string().c_str()
        );

        if (backgroundShader->ID == 0) {
            SDL_Log("Background Shader creation failed");
            return false;
        }

        backgroundShader->use();
        glUniform1i(
            glGetUniformLocation(backgroundShader->ID, "backgroundTexture"),
            0
        );

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindVertexArray(0);
        return true;
    }

    bool InitUI() {
        const float vertices[] = {
            // x, y,      u, v
            0.5f,  0.5f, 1.0f, 0.0f,  // top right
            0.5f, -0.5f, 1.0f, 1.0f,  // bottom right
           -0.5f, -0.5f, 0.0f, 1.0f,  // bottom left
           -0.5f,  0.5f, 0.0f, 0.0f   // top left
       };

        const unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
        };
        const fs::path shaderPath = ProjectManager::GetEngineBasePath() / "Shaders";
        uiShader = std::make_unique<Shader>(
    (shaderPath / "UI" / "UI.vs.glsl").string().c_str(),
    (shaderPath / "UI" / "UI.fs.glsl").string().c_str()
        );


        if (uiShader->ID == 0) {
            SDL_Log("UI Shader creation failed");
            return false;
        }

        glGenVertexArrays(1, &uiVAO);
        glGenBuffers(1, &uiVBO);
        glGenBuffers(1, &uiEBO);

        glBindVertexArray(uiVAO);

        glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // IMPORTANT: bind EBO while uiVAO is bound
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiEBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
        );

        // location 0: position
        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(float),
            reinterpret_cast<void*>(0)
        );
        glEnableVertexAttribArray(0);

        // location 1: UV
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            GL_FALSE,
            4 * sizeof(float),
            reinterpret_cast<void*>(2 * sizeof(float))
        );
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        return true;
    }

    bool InitDebug() {

        const fs::path shaderPath = ProjectManager::GetEngineBasePath() / "Shaders";
        debugShader = std::make_unique<Shader>(
    (shaderPath / "Debug" / "debug.vs.glsl").string().c_str(),
    (shaderPath / "Debug" / "debug.fs.glsl").string().c_str()
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        debugColorUniform = glGetUniformLocation(debugShader->ID, "uColor");
        if (debugColorUniform == -1) {
            SDL_Log("Failed to get debug shader uniform location uColor");
            return false;
        }
        return true;
    }

    bool InitText() {
        const fs::path shaderPath = ProjectManager::GetEngineBasePath() / "Shaders";
        textShader = std::make_unique<Shader>(
    (shaderPath / "Glyph" / "glyph.vs.glsl").string().c_str(),
    (shaderPath / "Glyph" / "glyph.fs.glsl").string().c_str()
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

        return true;
    }

    //endregion

    bool Initialize() {
        InitSDL();
        InitImGui();
        InitProjection();
        InitUI();
        if (DEBUG_ENABLED) InitDebug();
        InitText();

        SDL_SetWindowRelativeMouseMode(window, true);

        return true;
    }
}
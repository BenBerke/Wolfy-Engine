//
// Created by berke on 5/4/2026.
//
#include "Headers/Launcher/LauncherApp.hpp"
#include "Headers/Project/ProjectManager.hpp"

#include <iostream>
#include <SDL3/SDL.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

namespace {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    bool quitRequested = false;
}

namespace LauncherApp {
    void Start() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
            return;
        }

        if (!SDL_CreateWindowAndRenderer(
                "Tilky Engine Launcher",
                1060,
                800,
                0,
                &window,
                &renderer
            )) {
            std::cerr << "Failed to start launcher: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        io.Fonts->AddFontFromFileTTF(
            "../EngineAssets/Fonts/Notosans.ttf",
            18.0f
        );

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer3_Init(renderer);
    }

    void Update() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {
                quitRequested = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Tilky Engine Launcher");

        ImGui::Text("Projects");

        if (ImGui::Button("Create Project")) {
            std::cout << "Create Project clicked\n";
        }

        if (ImGui::Button("Open Project")) {
            std::cout << "Open Project clicked\n";
        }

        if (ImGui::Button("Quit")) {
            quitRequested = true;
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
    }

    bool QuitRequested() {
        return quitRequested;
    }

    void Destroy() {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        if (renderer != nullptr) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window != nullptr) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }
}
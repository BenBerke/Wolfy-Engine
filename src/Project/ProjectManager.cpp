//
// Created by berke on 5/3/2026.
//
#include "Headers/Project/ProjectManager.hpp"

#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
    bool projectLoaded = false;

    fs::path currentProjectFile;
    fs::path currentProjectFolder;
    fs::path currentAssetsPath;
    fs::path currentLevelsPath;
    fs::path currentTexturesPath;

    std::string currentProjectName;
}

namespace ProjectManager {
    bool OpenProject(const std::filesystem::path &projectFile) {
        if (!fs::exists(projectFile)) {
            std::cout << "Project file does not exist" << std::endl;
            return false;
        }

        std::ifstream file(projectFile);
        if (!file.is_open()) {
            std::cout << "Failed to open project" << std::endl;
            return false;
        }

        json projectData;
        try {
            file >> projectData;
        }
        catch (std::exception &e) {
            std::cout << "Failed to parse project file:" << e.what() << std::endl;
            return false;
        }

        currentProjectFile = projectFile;
        currentProjectFolder = projectFile.parent_path();
        currentProjectName = projectData.value("name", currentProjectFolder.filename().string());

        const std::string assetsFolder = projectData.value("assetsFolder", "Assets");

        currentAssetsPath = currentProjectFolder / assetsFolder;
        currentLevelsPath = currentAssetsPath / "Levels";
        currentTexturesPath = currentAssetsPath / "Textures";

        if (!fs::exists(currentAssetsPath)) {
            std::cout << "Missing Assets folder: " << currentAssetsPath << '\n';
            return false;
        }

        if (!fs::exists(currentLevelsPath)) {
            std::cout << "Missing Levels folder: " << currentLevelsPath << '\n';
            return false;
        }

        if (!fs::exists(currentTexturesPath)) {
            std::cout << "Missing Textures folder: " << currentTexturesPath << '\n';
            return false;
        }

        projectLoaded = true;
        return true;
    }

    bool HasProject() {
        return projectLoaded;
    }

    fs::path GetProjectFiles() {
        return currentProjectFile;
    }

    fs::path GetProjectFolder() {
        return currentProjectFolder;
    }

    fs::path GetAssetsPath() {
        return currentAssetsPath;
    }

    fs::path GetTexturesPath() {
        return currentTexturesPath;
    }

    fs::path GetLevelsPath() {
        return currentLevelsPath;
    }

    fs::path GetEngineBasePath() {
        const char* basePath = SDL_GetBasePath();

        if (basePath == nullptr) {
            return std::filesystem::current_path();
        }

        return std::filesystem::path(basePath);
    }
    std::string GetProjectName() {
        return currentProjectName;
    }
}
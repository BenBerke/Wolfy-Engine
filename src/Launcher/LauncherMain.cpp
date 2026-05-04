//
// Created by berke on 5/3/2026.
//
#include <iostream>
#include <filesystem>
#include <fstream>

#include "Headers/Launcher/LauncherApp.hpp"
#include "Headers/Project/ProjectManager.hpp"
#include "src/MapEditor/MapEditorInternal.hpp"

namespace fs = std::filesystem;

//region Project Creation

//endregion

int main() {
    const fs::path projectsPath = ProjectManager::GetDefaultProjectsFolder();

    try {
        fs::create_directories(projectsPath);
        std::cout << "Created/Exists: " << projectsPath << std::endl;
    }
    catch (std::exception& e) {
        std::cout << "Failed to create projects directory " << std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }

    LauncherApp::Start();

    while (!LauncherApp::QuitRequested()) {
        LauncherApp::Update();
    }

    LauncherApp::Destroy();
    // CreateDirectory(projectName);

    return 0;
}
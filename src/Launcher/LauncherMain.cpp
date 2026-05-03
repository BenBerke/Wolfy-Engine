//
// Created by berke on 5/3/2026.
//
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

fs::path GetUserHomeDirectory() {
#if _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
#else
    const char* userProfile = std::getenv("HOME");
#endif

    if (userProfile != nullptr) {
        return fs::path(userProfile);
    }
    return fs::current_path();
}

fs::path GetDefaultProjectsFolder() {
#ifdef _WIN32
    return GetUserHomeDirectory() / "Documents" / "Tilky Engine" / "Projects";
#else
    const char* xdgDataHome = std::getenv("XDG_DATA_HOME");

    if (xdgDataHome != nullptr) {
        return fs::path(xdgDataHome) / "Tilky Engine" / "Projects";
    }

    return GetUserHomeDirectory() / ".local" / "share" / "Tilky Engine" / "Projects";
#endif
}

void LaunchEngine(const fs::path &projectFile) {
    const std::string command = "Wolfy_Engine.exe --project \"" + projectFile.string() + "\"";
    std::system(command.c_str());
}

bool OpenProject(const fs::path& path) {
    const fs::path projectFile = path / "project.tilky";
    if (!fs::exists(projectFile)) {
        std::cout << "Missing .tilky file" << std::endl;
        return false;
    }
    LaunchEngine(projectFile);
}

void CreateProject(const fs::path &directory, const std::string& projectName) {
    const fs::path assetsPath = directory / "Assets";
    const fs::path levelsPath = assetsPath / "Levels";
    const fs::path texturesPath = assetsPath / "Textures";

    bool openProject = true;

    try {
        if (!fs::exists(assetsPath)) {
            if (!fs::create_directories(assetsPath)) {
                std::cout << "Could not create assets folder" << std::endl;
                openProject = false;
            }
        }
        if (!fs::exists(levelsPath)) {
            if (!fs::create_directories(levelsPath)) {
                std::cout << "Could not create levels folder" << std::endl;
                openProject = false;
            }
        }
        if (!fs::exists(texturesPath)) {
            if (!fs::create_directories(texturesPath)) {
                std::cout << "Could not create textures folder" << std::endl;
                openProject = false;
            }
        }

        const fs::path dataPath = directory / ("project.tilky");

        json projectData;
        projectData["name"] = projectName;
        projectData["assetsFolder"] = "Assets";

        std::ofstream file(dataPath);
        if (file.is_open()) {
            file << projectData.dump(4);
            file.close();
        }
        else std::cout << "Failed to create metadata " << projectName << std::endl;
    }
    catch (std::exception& e) {
        std::cout << "Failed to create project " << e.what() << std::endl;
    }

    if (!openProject) return;
    OpenProject(directory);
}

void CreateDirectory(const std::string &projectName) {
    const fs::path path = GetDefaultProjectsFolder() / projectName;
    try {
        if (!fs::exists(path)) {
            if (fs::create_directories(path)) {
                std::cout << "Folder created at " + path.string() << std::endl;
                CreateProject(path, projectName);
            }
            else std::cout << "Failed to create folder " << std::endl;
        }
        else {
            std::cout << "Folder already exists " << std::endl;
            OpenProject(path);
        }
    }
    catch (std::exception& e) {
        std::cout << "Failed to create project " <<  e.what() << std::endl;
    }
}

int main() {
    const fs::path projectsPath = GetDefaultProjectsFolder();

    try {
        fs::create_directories(projectsPath);
        std::cout << "Created/Exists: " << projectsPath << std::endl;
    }
    catch (std::exception& e) {
        std::cout << "Failed to create projects directory " << std::endl;
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::string input;
    std::cin >> input;

    CreateDirectory(input);

    return 0;
}
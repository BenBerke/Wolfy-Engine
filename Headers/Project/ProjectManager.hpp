//
// Created by berke on 5/3/2026.
//

#ifndef WOLFY_ENGINE_PROJECTMANAGER_H
#define WOLFY_ENGINE_PROJECTMANAGER_H

namespace fs = std::filesystem;

namespace ProjectManager {
    bool OpenProject(const std::filesystem::path &projectFile);

    bool HasProject();

    fs::path GetProjectFiles();
    fs::path GetProjectFolder();
    fs::path GetAssetsPath();
    fs::path GetTexturesPath();
    fs::path GetLevelsPath();
    fs::path GetEngineBasePath();
    std::string GetProjectName();

}

#endif //WOLFY_ENGINE_PROJECTMANAGER_H
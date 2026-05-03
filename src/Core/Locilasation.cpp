//
// Created by berke on 5/3/2026.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <SDL3/SDL_log.h>
#include "../../Headers/Core/Localisation.hpp"

using json = nlohmann::json;

namespace {
    std::unordered_map<std::string, std::string> strings;
    std::string currentLanguage = "en";

    const std::string missingString = "<missing>";
}

namespace Localisation {
    bool LoadLanguage(const std::string& languageCode) {
        const std::string path = "../Engine/Local/" + languageCode + ".json";

        std::ifstream file(path);
        if (!file.is_open()) {
            SDL_Log("Failed to open %s", path.c_str());
            return false;
        }

        json data;
        try {
            file >> data;
        }
        catch (const std::exception& e){
            SDL_Log("Failed to parse %s", e.what());
        }

        strings.clear();

        for (auto& [key, value] : data.items()) {
            if (value.is_string()) strings[key] = value.get<std::string>();
        }

        currentLanguage = languageCode;

        return true;
    }

    const std::string& Get(const std::string& key) {
        auto it = strings.find(key);
        if (it == strings.end()) {
            SDL_Log("Failed to find key %s", key.c_str());
            return missingString;
        };

        return it->second;
    }

    const std::string& CurrentLanguage() {
        return currentLanguage;
    }
}
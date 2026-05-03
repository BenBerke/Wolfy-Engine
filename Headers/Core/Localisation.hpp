//
// Created by berke on 5/3/2026.
//

#ifndef WOLFY_ENGINE_LOCALISATION_H
#define WOLFY_ENGINE_LOCALISATION_H

#include <string>
#include <unordered_map>

namespace Localisation {
    bool LoadLanguage(const std::string& languageCode);

    const std::string& Get(const std::string& key);

    const std::string& CurrentLanguage();
}

#endif //WOLFY_ENGINE_LOCALISATION_H
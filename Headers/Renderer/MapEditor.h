//
// Created by berke on 4/13/2026.
//

#ifndef WOLFY_ENGINE_MAPEDITOR_H
#define WOLFY_ENGINE_MAPEDITOR_H

#include <vector>
#include "../Objects/Wall.h"

namespace MapEditor {
    inline std::vector<Wall> walls;
    void AddWall(const Wall &wall);
}

#endif //WOLFY_ENGINE_MAPEDITOR_H
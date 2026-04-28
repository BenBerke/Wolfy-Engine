#pragma once

#include <string>
#include <vector>

#include "../Math/Vector/Vector2.hpp"
#include "../Math/Vector/Vector3.hpp"
#include "../Objects/Wall.hpp"
#include "../Objects/Sector.hpp"
#include "Headers/Objects/Object.h"

namespace MapEditor {
    extern std::vector<Wall> walls;
    extern std::vector<Sector> sectors;
    extern std::vector<Object> objects;
    extern Vector2 playerStartPos;

    void Start();
    void Update();
    void Destroy();

    bool QuitRequested();
    bool LoadLevel(const std::string& level);

    void AddWall(const Wall& wall);
    void AddSector(const Sector& sector);

    void CreateSector(
        const std::vector<Vector2>& vertices,
        float ceilHeight,
        float floorHeight,
        Vector3 ceilColor,
        Vector3 floorColor,
        int ceilTextureIndex,
        int floorTextureIndex
    );

    void TriangulateSectors();
    std::vector<Triangle> Triangulate(std::vector<Vector2> vertices);
}

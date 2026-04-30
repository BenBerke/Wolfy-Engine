#pragma once

#include <vector>

#include "Headers/Math/Vector/Vector2.hpp"

namespace Geometry {
    bool IsPointInPolygon(
        const std::vector<Vector2>& polygon,
        const Vector2& point
    );

    float PolygonAreaAbs(
        const std::vector<Vector2>& polygon
    );
}
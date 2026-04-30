#include "../../Headers/Math/Geometry/Geometry.h"

#include <cmath>

namespace Geometry {
    bool IsPointInPolygon(
        const std::vector<Vector2>& polygon,
        const Vector2& point
    ) {
        bool inside = false;
        const size_t n = polygon.size();

        if (n < 3) {
            return false;
        }

        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const bool isBetweenY =
                (polygon[i].y > point.y) != (polygon[j].y > point.y);

            if (
                isBetweenY &&
                point.x <
                    (polygon[j].x - polygon[i].x) *
                    (point.y - polygon[i].y) /
                    (polygon[j].y - polygon[i].y) +
                    polygon[i].x
            ) {
                inside = !inside;
            }
        }

        return inside;
    }

    float PolygonAreaAbs(
        const std::vector<Vector2>& polygon
    ) {
        float area = 0.0f;

        for (int i = 0; i < static_cast<int>(polygon.size()); ++i) {
            const int next = (i + 1) % static_cast<int>(polygon.size());

            area += polygon[i].x * polygon[next].y;
            area -= polygon[next].x * polygon[i].y;
        }

        return std::abs(area) * 0.5f;
    }
}
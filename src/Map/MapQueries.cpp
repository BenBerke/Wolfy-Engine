//
// Created by berke on 4/30/2026.
//

#include "../../Headers/Map/MapQueries.h"
#include "../../Headers/Math/Geometry/Geometry.h"

namespace MapQueries {
    int FindSectorContainingPoint(const std::vector<Sector>& sectors, const Vector2 position) {
        int bestSector = -1;
        float bestArea = std::numeric_limits<float>::max();

        for (int i = 0; i < static_cast<int>(sectors.size()); ++i) {
            if (!Geometry::IsPointInPolygon(sectors[i].vertices, position)) {
                continue;
            }

            const float area = Geometry::PolygonAreaAbs(sectors[i].vertices);

            if (area < bestArea) {
                bestArea = area;
                bestSector = i;
            }
        }

        return bestSector;
    }
}
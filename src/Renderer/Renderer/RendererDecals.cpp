#include "RendererInternal.hpp"

#include "Headers/Renderer/MapEditor.hpp"
#include "Headers/Objects/Object.h"
#include "Headers/Objects/Wall.hpp"

namespace RendererInternal {
    void BuildGpuDecals() {
        gpuDecals.clear();

        for (const Object& object : MapEditor::objects) {
            if (object.type != OBJ_DECAL) {
                continue;
            }

            if (object.wallIndex < 0 ||
                object.wallIndex >= static_cast<int>(MapEditor::walls.size())) {
                continue;
            }

            const Wall& wall = MapEditor::walls[object.wallIndex];

            const Vector2 wallVector = wall.end - wall.start;
            const float wallLength = std::sqrt(
                wallVector.x * wallVector.x +
                wallVector.y * wallVector.y
            );

            if (wallLength <= 0.0001f) {
                continue;
            }

            const Vector2 wallDir = {
                wallVector.x / wallLength,
                wallVector.y / wallLength
            };

           // const float halfWidth = object.width * 0.5f;
            const float halfWidth = 16.0f;

            Vector2 decalStart = {
                object.position.x - wallDir.x * halfWidth,
                object.position.y - wallDir.y * halfWidth
            };

            Vector2 decalEnd = {
                object.position.x + wallDir.x * halfWidth,
                object.position.y + wallDir.y * halfWidth
            };

            GpuDecal decal;

            decal.startEnd = {
                decalStart.x,
                decalStart.y,
                decalEnd.x,
                decalEnd.y
            };

            decal.color = {
                255.0f,
                255.0f,
                255.0f,
                255.0f
            };

            // decal.heights = {
            //     object.bottomHeight,
            //     object.bottomHeight + object.height,
            //     0.0f,
            //     0.0f
            // };

            decal.heights = {
                10,
                10 + 10,
                0.0f,
                0.0f
            };

            decal.data = {
                static_cast<float>(object.textureIndex),
                0.0f,
                0.0f,
                0.0f
            };

            gpuDecals.push_back(decal);
        }

        //region hardcode
        constexpr int wallIndex = 58;

        if (wallIndex >= 0 && wallIndex < static_cast<int>(MapEditor::walls.size())) {
            const Wall& wall = MapEditor::walls[wallIndex];

            const Vector2 wallVector = wall.end - wall.start;
            const float wallLength = std::sqrt(
                wallVector.x * wallVector.x +
                wallVector.y * wallVector.y
            );

            if (wallLength > 0.0001f) {
                const Vector2 wallDir = {
                    wallVector.x / wallLength,
                    wallVector.y / wallLength
                };

                const Vector2 wallMiddle = {
                    (wall.start.x + wall.end.x) * 0.5f,
                    (wall.start.y + wall.end.y) * 0.5f
                };

                constexpr float decalWidth = 32.0f;
                constexpr float halfDecalWidth = decalWidth * 0.5f;

                const Vector2 decalStart = {
                    wallMiddle.x - wallDir.x * halfDecalWidth,
                    wallMiddle.y - wallDir.y * halfDecalWidth
                };

                const Vector2 decalEnd = {
                    wallMiddle.x + wallDir.x * halfDecalWidth,
                    wallMiddle.y + wallDir.y * halfDecalWidth
                };

                GpuDecal decal;

                decal.startEnd = {
                    decalStart.x,
                    decalStart.y,
                    decalEnd.x,
                    decalEnd.y
                };

                decal.color = {
                    255.0f,
                    255.0f,
                    255.0f,
                    255.0f
                };

                decal.heights = {
                    20.0f, // bottom height
                    52.0f, // top height
                    0.0f,
                    0.0f
                };

                decal.data = {
                    static_cast<int>(4.0f), // texture index
                    0.0f,
                    0.0f,
                    0.0f
                };

                gpuDecals.push_back(decal);
            }
        }
        //endregion

        decalCount = static_cast<GLsizei>(gpuDecals.size());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, decalSSBO);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            gpuDecals.size() * sizeof(GpuDecal),
            gpuDecals.empty() ? nullptr : gpuDecals.data(),
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, decalSSBO);
    }
}
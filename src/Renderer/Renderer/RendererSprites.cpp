#include "RendererInternal.hpp"
#include "Headers/Map/MapQueries.h"
#include "Headers/Renderer/MapEditor.hpp"

namespace RendererInternal {
    void BuildGpuSprites() {
        gpuSprites.clear();

        for (Object& object : MapEditor::objects) {
            if (object.type != OBJ_SPRITE) {
                continue;
            }

            UpdateObjectSector(object, MapEditor::sectors);

            if (object.sectorIndex < 0 ||
                object.sectorIndex >= static_cast<int>(MapEditor::sectors.size())) {
                continue;
                }

            const float bottomHeight =
                GetObjectBottomHeight(object, MapEditor::sectors);

            GpuSprite gpuSprite;

            gpuSprite.positionSize = {
                object.position.x,
                object.position.y,
                bottomHeight,
                object.height
            };

            gpuSprite.color = {
                255.0f,
                255.0f,
                255.0f,
                255.0f
            };

            gpuSprite.data = {
                object.width,
                static_cast<float>(object.textureIndex),
                0.0f,
                0.0f
            };

            gpuSprites.push_back(gpuSprite);
        }

        spriteCount = static_cast<GLsizei>(gpuSprites.size());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, spriteSSBO);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            gpuSprites.size() * sizeof(GpuSprite),
            gpuSprites.empty() ? nullptr : gpuSprites.data(),
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, spriteSSBO);
    }
}

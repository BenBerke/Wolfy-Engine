#include "RendererInternal.hpp"
#include "Headers/Renderer/MapEditor.hpp"

namespace RendererInternal {
    void BuildGpuSprites() {
        gpuSprites.clear();

        for (const Object& object : MapEditor::objects) {
            if (object.type == OBJ_PLAYER_SPAWN) {
                continue;
            }

            if (object.type == OBJ_SPRITE) {
                GpuSprite sprite;

                sprite.positionSize = {
                    object.position.x,
                    object.position.y,
                    0.0f,   // bottom height
                    64.0f   // sprite height
                };

                sprite.color = {
                    255.0f,
                    255.0f,
                    255.0f,
                    255.0f
                };

                sprite.data = {
                    32.0f, // sprite width
                    2.0f,  // texture index for now
                    0.0f,
                    0.0f
                };

                gpuSprites.push_back(sprite);
            }
        }

        spriteCount = static_cast<GLsizei>(gpuSprites.size());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, spriteSSBO);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            gpuSprites.size() * sizeof(GpuSprite),
            gpuSprites.data(),
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, spriteSSBO);
    }
}

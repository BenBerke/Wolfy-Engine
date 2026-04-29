#include "RendererInternal.hpp"

#include "Headers/Renderer/MapEditor.hpp"

namespace RendererInternal {
    void BuildGpuSectors() {
        gpuSectors.clear();
        gpuSectors.reserve(MapEditor::sectors.size());

        for (const Sector& sector : MapEditor::sectors) {
            GpuSector gpuSector;

            gpuSector.heights = {
                sector.floorHeight,
                sector.ceilingHeight,
                0.0f,
                0.0f
            };

            gpuSector.floorColor = {
                sector.floorColor.x,
                sector.floorColor.y,
                sector.floorColor.z,
                255.0f
            };

            gpuSector.ceilingColor = {
                sector.ceilingColor.x,
                sector.ceilingColor.y,
                sector.ceilingColor.z,
                255.0f
            };

            gpuSector.textureData = {
                static_cast<float>(sector.floorTextureIndex),
                0.0,
                0.0f,
                0.0f
            };

            gpuSectors.push_back(gpuSector);
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, sectorSSBO);

        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            gpuSectors.size() * sizeof(GpuSector),
            gpuSectors.empty() ? nullptr : gpuSectors.data(),
            GL_DYNAMIC_DRAW
        );

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, sectorSSBO);
    }
}
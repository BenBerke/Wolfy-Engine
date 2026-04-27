#include "MapEditorInternal.hpp"

#include "imgui.h"

#include <array>
#include <string>
#include <vector>

#include "Headers/Renderer/TextureManager.hpp"

namespace MapEditorInternal {
    const char* GetModeName(const Mode mode) {
        switch (mode) {
            case MODE_DOT:
                return "Dot Mode";

            case MODE_WALL:
                return "Wall Mode";

            case MODE_SECTOR:
                return "Sector Mode";

            case MODE_OBJECT:
                return "Object Mode";

            default:
                return "Unknown Mode";
        }
    }

    void MoveMode() {
        currentMode = static_cast<Mode>((currentMode + 1) % MODE_COUNT);
    }

    void DrawEditorUI() {
        ImGui::Begin("Editor");

        if (ImGui::Button("Mode")) {
            const Mode previousMode = currentMode;

            MoveMode();

            if (previousMode == MODE_SECTOR) {
                FinishSectorSelection();
                creatableSector = false;
            }

            if (currentMode == MODE_SECTOR) {
                sectorBeingCreated.clear();
                creatableSector = false;
            }
        }

        ImGui::Text("%s", GetModeName(currentMode));

        ImGui::Text("\nDots: %d", static_cast<int>(placedCorners.size()));
        ImGui::Text("Lines: %d", static_cast<int>(MapEditor::walls.size()));
        ImGui::Text("Sectors: %d", static_cast<int>(MapEditor::sectors.size()));

        if (editingSector && currentMode == MODE_SECTOR && selectedSector != -1) {
            ImGui::Begin("Sector", &editingSector);

            float ceilHeight = MapEditor::sectors[selectedSector].ceilingHeight;
            float floorHeight = MapEditor::sectors[selectedSector].floorHeight;

            int floorTexture = MapEditor::sectors[selectedSector].floorTextureIndex;
            int ceilTexture = MapEditor::sectors[selectedSector].ceilingTextureIndex;

            Vector3 ceilColor = MapEditor::sectors[selectedSector].ceilingColor;
            Vector3 floorColor = MapEditor::sectors[selectedSector].floorColor;

            ImGui::InputFloat("Ceil Height", &ceilHeight);
            ImGui::InputFloat("Floor Height", &floorHeight);
            ImGui::InputInt("Floor Texture Index", &floorTexture);
            ImGui::InputInt("Ceiling Texture Index", &ceilTexture);
            ImGui::InputFloat3("Ceiling Color", &ceilColor.x);
            ImGui::InputFloat3("Floor Color", &floorColor.x);

            MapEditor::sectors[selectedSector].ceilingHeight = ceilHeight;
            MapEditor::sectors[selectedSector].floorHeight = floorHeight;
            MapEditor::sectors[selectedSector].floorTextureIndex = floorTexture;
            MapEditor::sectors[selectedSector].ceilingTextureIndex = ceilTexture;
            MapEditor::sectors[selectedSector].ceilingColor = ceilColor;
            MapEditor::sectors[selectedSector].floorColor = floorColor;

            if (ImGui::Button("Delete")) {
                MapEditor::sectors.erase(MapEditor::sectors.begin() + selectedSector);
                editingSector = false;
            }

            if (ImGui::Button("Close")) {
                editingSector = false;
            }

            ImGui::End();
        }

        if (editingWall && currentMode == MODE_WALL && selectedWall != -1) {
            ImGui::Begin("Wall", &editingSector);

            Vector4 color = MapEditor::walls[selectedWall].color;

            int frontSector = MapEditor::walls[selectedWall].frontSector;
            int backSector = MapEditor::walls[selectedWall].backSector;
            int textureIndex = MapEditor::walls[selectedWall].textureIndex;

            ImGui::InputInt("Front Sector", &frontSector);
            ImGui::InputInt("Back Sector", &backSector);
            ImGui::InputInt("Texture Index", &textureIndex);

            ImGui::InputFloat4("Wall Color", &color.x);

            MapEditor::walls[selectedWall].color = color;
            MapEditor::walls[selectedWall].textureIndex = textureIndex;
            MapEditor::walls[selectedWall].frontSector = frontSector;
            MapEditor::walls[selectedWall].backSector = backSector;

            if (ImGui::Button("Delete")) {
                MapEditor::walls.erase(MapEditor::walls.begin() + selectedWall);
                editingWall = false;
            }

            if (ImGui::Button("Close")) {
                editingWall = false;
            }

            ImGui::End();
        }

        if (creatableSector) {
            if (ImGui::Button("Create Sector")) {
                if (sectorBeingCreated.size() >= 3) {
                    if (!SamePoint(sectorBeingCreated.front(), sectorBeingCreated.back())) {
                        sectorBeingCreated.push_back(sectorBeingCreated.front());
                    }

                    FinishSectorSelection();

                    creatableSector = false;
                }
            }
        }

        if (ImGui::Button("Create Texture")) {
            textureInputs.push_back({});
        }

        for (int i = 0; i < static_cast<int>(textureInputs.size()); ++i) {
            ImGui::PushID(i);

            std::string label = "Texture " + std::to_string(i);
            ImGui::InputText(label.c_str(), textureInputs[i].data(), textureInputs[i].size());

            ImGui::PopID();
        }

        if (ImGui::Button("Save & Play")) {
            if (SaveAndQuit()) {
                quit = true;
            }
        }

        ImGui::End();
    }
}

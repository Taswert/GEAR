#include "EditorUtilsModule.hpp"
#include "razoomUtils.hpp"



void ErGui::renderEditorUtilsModule() {
    if (!ImGui::Begin("Editor-Utils")) {
        ImGui::End();
        return;
    }

    // Selections

    if (ImGui::Button("Select All")) {
        getFakePauseLayer()->onSelectAll(nullptr);
    }

    if (ImGui::Button("Select All Right")) {
        getFakePauseLayer()->onSelectAllRight(nullptr);
    }

    if (ImGui::Button("Select All Left")) {
        getFakePauseLayer()->onSelectAllLeft(nullptr);
    }

    ImGui::Dummy({ 5.f, 5.f }); // Create / Edit

    if (ImGui::Button("Align-X")) {
        getFakePauseLayer()->onAlignX(nullptr);
    }

    if (ImGui::Button("Align-Y")) {
        getFakePauseLayer()->onAlignY(nullptr);
    }

    if (ImGui::Button("Create Loop")) {
        getFakePauseLayer()->onCreateLoop(nullptr);
    }

    if (ImGui::Button("Create Extras")) {
        getFakePauseLayer()->onCreateExtras(nullptr);
    }

    ImGui::Dummy({ 5.f, 5.f }); // Group Change

    if (ImGui::Button("Build Helper")) {
        getFakePauseLayer()->onBuildHelper(nullptr);
    }

    if (ImGui::Button("Re-Group")) {
        getFakePauseLayer()->onReGroup(nullptr);
    }

    if (ImGui::Button("New Group X")) {
        getFakePauseLayer()->onNewGroupX(nullptr);
    }

    if (ImGui::Button("New Group Y")) {
        getFakePauseLayer()->onNewGroupY(nullptr);
    }

    ImGui::Dummy({ 5.f, 5.f }); // Other

    if (ImGui::Button("Copy + Color")) {
        getFakePauseLayer()->onCopyWColor(nullptr);
    }

    if (ImGui::Button("Paste + Color")) {
        getFakePauseLayer()->onPasteWColor(nullptr);
    }

    if (ImGui::Button("Unlock Layers")) {
        getFakePauseLayer()->onUnlockAllLayers(nullptr);
    }

    if (ImGui::Button("Reset Unused")) {
        // getFakePauseLayer()->onResetUnusedColors(nullptr);
        createQuickPopup("Reset Unused Colors", "Reset all unused color channels?\n"
            "<cy>This will make those channels available as 'next free'</c>", "NO", "YES", 300.f, 
            [](auto, bool isBtn2) {
                if (isBtn2) {
                    LevelEditorLayer::get()->resetUnusedColorChannels();
                }
            }, 
            true, true
        );
    }

    if (ImGui::Button("Uncheck Portals")) {
        getFakePauseLayer()->uncheckAllPortals(nullptr);
    }

    ImGui::End();
}

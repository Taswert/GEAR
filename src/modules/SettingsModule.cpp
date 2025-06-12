#include "SettingsModule.hpp"
#include "razoomUtils.hpp"

bool previewAnimations;
bool previewParticles;
bool previewShaders;
bool ldm;
bool previewMode;
bool previewBpm;
bool showPos;
bool durationLines;
bool effectLines;
bool showGrid;
bool showGround;
bool showHitboxes;


void ErGui::renderSettingsModule() {
    if (!ImGui::Begin("Settings-Module")) {
        ImGui::End();
        return;
    }

    auto gm = GameManager::get();

    previewAnimations = gm->getGameVariable("0118");
    previewParticles = gm->getGameVariable("0117");
    previewShaders = gm->getGameVariable("0158");
    // ldm
    previewMode = gm->getGameVariable("0036");
    previewBpm = gm->m_showSongMarkers;
    // showPos
    durationLines = gm->getGameVariable("0058");
    effectLines = gm->getGameVariable("0043");
    showGrid = gm->getGameVariable("0038");
    showGround = gm->getGameVariable("0037");
    showHitboxes = gm->getGameVariable("0045");
    
    if (ImGui::Checkbox("Preview animations", &previewAnimations)) {
        // gm->setGameVariable("0118", previewAnimations);
        getFakePauseLayer()->togglePreviewAnim(nullptr);
    }

    if (ImGui::Checkbox("Preview particles", &previewParticles)) {
        gm->setGameVariable("0117", previewParticles);
        LevelEditorLayer::get()->updatePreviewParticles();
    }

    if (ImGui::Checkbox("Preview shaders", &previewShaders)) {
        gm->setGameVariable("0158", previewShaders);
        LevelEditorLayer::get()->updateOptions();
    }

    // if (ImGui::Checkbox("LDM", &ldm)) {
        
    // }

    if (ImGui::Checkbox("Preview mode", &previewMode)) {
        gm->setGameVariable("0036", previewMode);
        auto lel = LevelEditorLayer::get();
        if (lel->m_playbackMode != PlaybackMode::Not) {
            EditorUI::get()->onStopPlaytest(EditorUI::get()->m_playtestBtn);
        }
        lel->updateEditorMode();
        lel->updateOptions();
    }

    if (ImGui::Checkbox("BPM", &previewBpm)) {
        gm->m_showSongMarkers = previewBpm;
    }

    // if (ImGui::Checkbox("Pos", &showPos)) {
        
    // }

    if (ImGui::Checkbox("Duration lines", &durationLines)) {
        gm->setGameVariable("0058", durationLines);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Effect lines", &effectLines)) {
        gm->setGameVariable("0043", effectLines);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show grid", &showGrid)) {
        gm->setGameVariable("0038", showGrid);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show ground", &showGround)) {
        gm->setGameVariable("0037", showGround);
        LevelEditorLayer::get()->m_groundLayer->setVisible(showGround);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show hitboxes", &showHitboxes)) {
        gm->setGameVariable("0045", showHitboxes);
        LevelEditorLayer::get()->updateOptions();
    }

    ImGui::End();
}

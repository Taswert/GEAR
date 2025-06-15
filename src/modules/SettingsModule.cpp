#include "SettingsModule.hpp"
#include "razoomUtils.hpp"
#include <Geode/modify/EditorUI.hpp>

bool previewAnimations;
bool previewParticles;
bool previewShaders;
bool ldm;
bool previewMode;
bool songGuidelines;
bool showPos;
bool durationLines;
bool effectLines;
bool showGrid;
bool showGround;
bool showHitboxes;
bool showInvisible;
bool showPath;
bool showClicks;
bool showBackground;
bool showParticleIcons;


void ErGui::renderSettingsModule() {
    if (!ImGui::Begin("View")) {
        ImGui::End();
        return;
    }

    auto gm = GameManager::get();

    previewAnimations = gm->getGameVariable("0118");
    previewParticles =  gm->getGameVariable("0117");
    previewShaders =    gm->getGameVariable("0158");
    // ldm
    previewMode =       gm->getGameVariable("0036");
    songGuidelines =    gm->m_showSongMarkers;
    // showPos
    durationLines =     gm->getGameVariable("0058");
    effectLines =       gm->getGameVariable("0043");
    showGrid =          gm->getGameVariable("0038");
    showGround =        gm->getGameVariable("0037");
    showHitboxes =      gm->getGameVariable("0045");

    showInvisible =     !gm->getGameVariable("0121");
    showPath =          !gm->getGameVariable("0152");
    showClicks =        gm->getGameVariable("0149");
    showBackground =    !gm->getGameVariable("0078");
    showParticleIcons = !gm->getGameVariable("0137");


    ImGui::SeparatorText("Previews");

    if (ImGui::Checkbox("Preview Animations", &previewAnimations)) {
        // gm->setGameVariable("0118", previewAnimations);
        getFakePauseLayer()->togglePreviewAnim(nullptr);
    }

    if (ImGui::Checkbox("Preview Particles", &previewParticles)) {
        gm->setGameVariable("0117", previewParticles);
        LevelEditorLayer::get()->updatePreviewParticles();
    }

    if (ImGui::Checkbox("Preview Shaders", &previewShaders)) {
        gm->setGameVariable("0158", previewShaders);
        LevelEditorLayer::get()->updateOptions();
    }

    // if (ImGui::Checkbox("LDM", &ldm)) {
        
    // }

    if (ImGui::Checkbox("Preview Mode", &previewMode)) {
        gm->setGameVariable("0036", previewMode);
        auto lel = LevelEditorLayer::get();
        if (lel->m_playbackMode != PlaybackMode::Not) {
            EditorUI::get()->onStopPlaytest(EditorUI::get()->m_playtestBtn);
        }
        lel->updateEditorMode();
        lel->updateOptions();
    }

    ImGui::SeparatorText("Draws");

    if (ImGui::Checkbox("Show Grid", &showGrid)) {
        gm->setGameVariable("0038", showGrid);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Song Guidelines", &songGuidelines)) {
        gm->m_showSongMarkers = songGuidelines;
    }

    // Ugly Looking :(
    //ImGui::Text("BPM Guidelines");
    //ImGui::SameLine();
    //if (ImGui::Button("Enable All##BPMGuidelines")) {
    //    auto lel = LevelEditorLayer::get();
    //    for (auto alObj : lel->m_drawGridLayer->m_audioLineObjects) {
    //        alObj.second->m_disabled = false;
    //    }
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Disable All##BPMGuidelines")) {
    //    auto lel = LevelEditorLayer::get();
    //    for (auto alObj : lel->m_drawGridLayer->m_audioLineObjects) {
    //        alObj.second->m_disabled = true;
    //    }
    //}
    
    // if (ImGui::Checkbox("Pos", &showPos)) {
        
    // }

    if (ImGui::Checkbox("Duration Lines", &durationLines)) {
        gm->setGameVariable("0058", durationLines);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Effect Lines", &effectLines)) {
        gm->setGameVariable("0043", effectLines);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show Path", &showPath)) {
        gm->setGameVariable("0152", !showPath);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show Clicks", &showClicks)) {
        gm->setGameVariable("0149", showClicks);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show Hitboxes", &showHitboxes)) {
        gm->setGameVariable("0045", showHitboxes);
        LevelEditorLayer::get()->updateOptions();
    }

    ImGui::SeparatorText("Grounds");

    if (ImGui::Checkbox("Show Ground", &showGround)) {
        gm->setGameVariable("0037", showGround);
        LevelEditorLayer::get()->m_groundLayer->setVisible(showGround);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show Background", &showBackground)) {
        gm->setGameVariable("0078", !showBackground);
        LevelEditorLayer::get()->updateOptions();
    }
    
    ImGui::SeparatorText("Objects");

    if (ImGui::Checkbox("Show Invisible", &showInvisible)) {
        gm->setGameVariable("0121", !showInvisible);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Show Only LDM", &showLdm)) {
        for (auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->m_objects)) {
            if (obj->m_isHighDetail) {
                obj->setVisible(!showLdm);
            }
        }
    }

    if (ImGui::Checkbox("Show Particle Icons", &showParticleIcons)) {
        gm->setGameVariable("0137", !showParticleIcons);
        LevelEditorLayer::get()->updateOptions();

        for (const auto obj : CCArrayExt<GameObject*>(LevelEditorLayer::get()->m_objects)) {

            if (const auto pObj = dynamic_cast<ParticleGameObject*>(obj)) {
                // I Hope this will not cause any troubles lol
                if (showParticleIcons) {
                    static_cast<CCSprite*>(static_cast<CCSprite*>(pObj->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setOpacity(50);
                    static_cast<CCSprite*>(pObj->getChildren()->objectAtIndex(1))->setOpacity(50);
                }
                else {
                    static_cast<CCSprite*>(static_cast<CCSprite*>(pObj->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setOpacity(0);
                    static_cast<CCSprite*>(pObj->getChildren()->objectAtIndex(1))->setOpacity(0);
                }
            }
        }
    }

    ImGui::End();
}


// why tf this doesn't work?????
class $modify(EditorUI) {
    CCArray* pasteObjects(gd::string p0, bool p1, bool p2) {
        auto objArr = EditorUI::pasteObjects(p0, p1, p2);

        for (auto obj : CCArrayExt<GameObject*>(objArr)) {
            if (obj->m_isHighDetail) {
                obj->setVisible(!ErGui::showLdm);
                std::cout << "Visible: " << obj->isVisible() << " " << !ErGui::showLdm << "\n";
            }
        }

        return objArr;
    }

    bool init(LevelEditorLayer* lel) {
        bool ret = EditorUI::init(lel);

        for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
            if (obj->m_isHighDetail) {
                obj->setVisible(!ErGui::showLdm);
            }
        }

        return ret;
    }
};
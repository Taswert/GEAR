#include "SettingsModule.hpp"
#include "razoomUtils.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/DrawGridLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GJMGLayer.hpp>

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

bool showCenter;
bool showDashOrbs;
bool showMG;

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

    showCenter =    Mod::get()->getSavedValue<bool>("show-center");
    showDashOrbs =  Mod::get()->getSavedValue<bool>("show-dash-orbs");
    showMG =        Mod::get()->getSavedValue<bool>("show-mg");

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

    if (ImGui::Checkbox("Grid", &showGrid)) {
        gm->setGameVariable("0038", showGrid);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Guidelines", &songGuidelines)) {
        gm->m_showSongMarkers = songGuidelines;
    }

    if (ImGui::Checkbox("Center", &showCenter)) {
        Mod::get()->setSavedValue("show-center", showCenter);
    }

    if (ImGui::Checkbox("Dash Orbs", &showDashOrbs)) {
        Mod::get()->setSavedValue("show-dash-orbs", showDashOrbs);
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

    if (ImGui::Checkbox("Path", &showPath)) {
        gm->setGameVariable("0152", !showPath);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Clicks", &showClicks)) {
        gm->setGameVariable("0149", showClicks);
        LevelEditorLayer::get()->updateOptions();
    }

    if (ImGui::Checkbox("Hitboxes", &showHitboxes)) {
        gm->setGameVariable("0045", showHitboxes);
        LevelEditorLayer::get()->updateOptions();
    }

    ImGui::SeparatorText("Grounds");

    if (ImGui::Checkbox("Ground", &showGround)) {
        gm->setGameVariable("0037", showGround);
        LevelEditorLayer::get()->m_groundLayer->setVisible(showGround);
        LevelEditorLayer::get()->updateOptions();
    }


    if (auto gjmg = dynamic_cast<GJMGLayer*>(dynamic_cast<CCNode*>(LevelEditorLayer::get()->m_objectLayer->getParent())->getChildren()->objectAtIndex(1))) {
        if (ImGui::Checkbox("Middleground", &showMG)) {
            Mod::get()->setSavedValue("show-mg", showMG);
            gjmg->setVisible(showMG);
        }
    }

    if (ImGui::Checkbox("Background", &showBackground)) {
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

class $modify(GJMGLayer) {
    bool init(int p0) {
        if (!GJMGLayer::init(p0)) return false;

        this->setVisible(Mod::get()->getSavedValue<bool>("show-mg"));

        return true;
    }
};


class $modify(DashOrbLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        Ref<CCArray> dashOrbs = CCArray::create();
    };

    $override
    void addSpecial(GameObject* obj) {
        LevelEditorLayer::addSpecial(obj);
        if (obj->m_objectID == 1704 || obj->m_objectID == 1751) {
            m_fields->dashOrbs->addObject(obj);
        }
    }

    $override
    void removeSpecial(GameObject* obj) {
        LevelEditorLayer::removeSpecial(obj);
        if (obj->m_objectID == 1704 || obj->m_objectID == 1751) {
            m_fields->dashOrbs->removeObject(obj);
        }
    }
};


class $modify(DrawGridLayer) {
    void draw() {
        // show center
        bool tmp = m_editorLayer->m_previewMode;
        float tmp2 = m_editorLayer->m_editorUI->m_toolbarHeight;
        m_editorLayer->m_previewMode = showCenter;
        m_editorLayer->m_editorUI->m_toolbarHeight = 0;
        DrawGridLayer::draw();
        m_editorLayer->m_previewMode = tmp;
        m_editorLayer->m_editorUI->m_toolbarHeight = tmp2;

        // orbs
        if (showDashOrbs) {
            auto orbs = reinterpret_cast<DashOrbLevelEditorLayer*>(m_editorLayer)->m_fields->dashOrbs;
            for (int i = 0; i < orbs->count(); i++) {
                auto const orb = static_cast<GameObject*>(orbs->objectAtIndex(i));
                auto const begin = orb->getPosition();
                auto end = begin + ccp(
                    400 * cos(-orb->getRotation() / 180.f * M_PI),
                    400 * sin(-orb->getRotation() / 180.f * M_PI)
                );
                ccDrawColor4B(orb->m_objectID == 1704 ? ccc4(0, 255, 0, 255) : ccc4(255, 90, 180, 255));
                ccDrawLine(begin, end);
            }
        }

        // pulse trigger in/hold/out
        if (durationLines) {
            ccDrawColor4B(100, 100, 100, 75);
            for (auto* obj : CCArrayExt<EffectGameObject*>(m_effectGameObjects)) {
                if (obj->m_objectID == 1006) {
                    float const durs[] = {
                        obj->m_fadeInDuration,
                        obj->m_fadeInDuration + obj->m_holdDuration,
                        obj->m_fadeInDuration + obj->m_holdDuration + obj->m_fadeOutDuration
                    };
                    for (int i = 0; i < 3; i++) {
                        auto currTime = timeForPos(obj->getPosition(), obj->m_ordValue, obj->m_channelValue, 0, 1, 0, 0);
                        auto durPos = posForTime(currTime + durs[i]);
                        ccDrawFilledCircle(ccp(durPos.x, obj->getPositionY()), 1.5, 0, 10);
                    }
                }
            }
        }
    }
};


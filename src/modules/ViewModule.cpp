#include "ViewModule.hpp"
#include "myUtils.hpp"
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

void ErGui::renderViewModule() {
    if (!ImGui::Begin("View")) {
        ImGui::End();
        return;
    }

    auto gm = GameManager::get();
    auto lel = LevelEditorLayer::get();
    auto editorUI = EditorUI::get();
    auto mod = Mod::get();

    previewAnimations = gm->getGameVariable("0118");
    previewParticles = gm->getGameVariable("0117");
    previewShaders = gm->getGameVariable("0158");
    // ldm
    previewMode = gm->getGameVariable("0036");
    songGuidelines = gm->m_showSongMarkers;
    // showPos
    durationLines = gm->getGameVariable("0058");
    effectLines = gm->getGameVariable("0043");
    showGrid = gm->getGameVariable("0038");
    showGround = gm->getGameVariable("0037");
    showHitboxes = gm->getGameVariable("0045");

    showInvisible = !gm->getGameVariable("0121");
    showPath = !gm->getGameVariable("0152");
    showClicks = gm->getGameVariable("0149");
    showBackground = !gm->getGameVariable("0078");
    showParticleIcons = !gm->getGameVariable("0137");

    showCenter = mod->getSavedValue<bool>("show-center");
    showDashOrbs = mod->getSavedValue<bool>("show-dash-orbs");
    showMG = mod->getSavedValue<bool>("show-mg");

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
        lel->updateOptions();
    }

    // if (ImGui::Checkbox("LDM", &ldm)) {

    // }

    if (ImGui::Checkbox("Preview Mode", &previewMode)) {
        gm->setGameVariable("0036", previewMode);
        if (lel->m_playbackMode != PlaybackMode::Not) {
            editorUI->onStopPlaytest(editorUI->m_playtestBtn);
        }
        lel->updateEditorMode();
        lel->updateOptions();
    }

    ImGui::SeparatorText("Draws");


    // ToDo: Saving
    //float fGridColor[4] = { ErGui::gridColor[0] / 255.f, ErGui::gridColor[1] / 255.f, ErGui::gridColor[2] / 255.f, ErGui::gridColor[3] / 255.f };
    //if (ImGui::ColorButton("##GridColor", ImVec4(fGridColor[0], fGridColor[1], fGridColor[2], fGridColor[3]))) {
    //    ImGui::OpenPopup("GridColorPopup");
    //}

    //if (ImGui::BeginPopup("GridColorPopup")) {
    //    ImGuiColorEditFlags flags =
    //        ImGuiColorEditFlags_AlphaBar;
    //    if (geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel"))
    //        flags |= ImGuiColorEditFlags_PickerHueWheel;
    //    if (!geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel"))
    //        flags |= ImGuiColorEditFlags_DisableWheelRot;

    //    ImGui::ColorPicker4("Grid Color", fGridColor, flags);
    //    
    //    ErGui::gridColor[0] = int(fGridColor[0] * 255);
    //    ErGui::gridColor[1] = int(fGridColor[1] * 255);
    //    ErGui::gridColor[2] = int(fGridColor[2] * 255);
    //    ErGui::gridColor[3] = int(fGridColor[3] * 255);

    //    ImGui::EndPopup();
    //}

    //ImGui::SameLine();
    //ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 4.f);
    //ImGui::DragFloat("##GridWidth", &ErGui::gridWidth, 0.1f, 0.f, 10.f, "%.2f");
    //ImGui::SameLine();
    
    if (ImGui::Checkbox("Grid", &showGrid)) {
        gm->setGameVariable("0038", showGrid);
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Guidelines", &songGuidelines)) {
        gm->m_showSongMarkers = songGuidelines;
    }

    if (ImGui::Checkbox("Center", &showCenter)) {
        mod->setSavedValue("show-center", showCenter);
    }

    if (ImGui::Checkbox("Dash Orbs", &showDashOrbs)) {
        mod->setSavedValue("show-dash-orbs", showDashOrbs);
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
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Effect Lines", &effectLines)) {
        gm->setGameVariable("0043", effectLines);
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Path", &showPath)) {
        gm->setGameVariable("0152", !showPath);
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Clicks", &showClicks)) {
        gm->setGameVariable("0149", showClicks);
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Hitboxes", &showHitboxes)) {
        gm->setGameVariable("0045", showHitboxes);
        lel->updateOptions();
    }

    ImGui::SeparatorText("Grounds");

    if (ImGui::Checkbox("Ground", &showGround)) {
        gm->setGameVariable("0037", showGround);
        lel->m_groundLayer->setVisible(showGround);
        lel->updateOptions();
    }


    if (auto gjmg = typeinfo_cast<GJMGLayer*>(typeinfo_cast<CCNode*>(lel->m_objectLayer->getParent())->getChildren()->objectAtIndex(1))) {
        if (ImGui::Checkbox("Middleground", &showMG)) {
            mod->setSavedValue("show-mg", showMG);
            gjmg->setVisible(showMG);
        }
    }

    if (ImGui::Checkbox("Background", &showBackground)) {
        gm->setGameVariable("0078", !showBackground);
        lel->updateOptions();
    }
    
    ImGui::SeparatorText("Objects");

    if (ImGui::Checkbox("Show Invisible", &showInvisible)) {
        gm->setGameVariable("0121", !showInvisible);
        lel->updateOptions();
    }

    if (ImGui::Checkbox("Show Only LDM", &showLdm)) {
        for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
            if (obj->m_isHighDetail) {
                obj->setVisible(!showLdm);
            }
        }
    }

    if (ImGui::Checkbox("Show Particle Icons", &showParticleIcons)) {
        gm->setGameVariable("0137", !showParticleIcons);
        lel->updateOptions();

        for (const auto& obj : CCArrayExt<GameObject*>(lel->m_objects)) {

            if (const auto pObj = typeinfo_cast<ParticleGameObject*>(obj)) {
                // I Hate this so muuuuuuuuuuuuuuuuuuuuuuuuuuuuuch
                if (showParticleIcons) {
                    auto arr1 = pObj->getChildrenExt();
                    if (arr1.size() > 0) {
                        if (auto spr1 = static_cast<CCSprite*>(arr1[0])) {
                            auto arr2 = spr1->getChildrenExt();
                            if (arr2.size() > 0) {
                                if (auto spr2 = static_cast<CCSprite*>(arr2[0])) {
                                    spr2->setOpacity(50);
                                }
                            }
                        }
                    }
                    if (arr1.size() > 1) {
                        if (auto spr = static_cast<CCSprite*>(arr1[1])) 
                            spr->setOpacity(50);
                    }
                }
                else {
                    auto arr1 = pObj->getChildrenExt();
                    if (arr1.size() > 0) {
                        if (auto spr1 = static_cast<CCSprite*>(arr1[0])) {
                            auto arr2 = spr1->getChildrenExt();
                            if (arr2.size() > 0) {
                                if (auto spr2 = static_cast<CCSprite*>(arr2[0])) {
                                    spr2->setOpacity(0);
                                }
                            }
                        }
                    }
                    if (arr1.size() > 1) {
                        if (auto spr = static_cast<CCSprite*>(arr1[1]))
                            spr->setOpacity(0);
                    }
                }
            }
        }
    }

    ImGui::End();
}


class $modify(EditorUI) {
    CCArray* pasteObjects(gd::string p0, bool p1, bool p2) {
        auto objArr = EditorUI::pasteObjects(p0, p1, p2);

        for (auto obj : CCArrayExt<GameObject*>(objArr)) {
            if (obj->m_isHighDetail) {
                obj->setVisible(!ErGui::showLdm);
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


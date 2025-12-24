#include "LayerModule.hpp"
#include <imgui_internal.h>
#include <algorithm>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include "IconsMaterialDesignIcons.h"
#include "myUtils.hpp"
#include "Geode/modify/EditorUI.hpp"
#include "PropertiesModule.hpp"

using namespace geode::prelude;


void ErGui::LayerState::update() {
    auto lel = LevelEditorLayer::get();

    for (auto& [id, layer] : layers) {
        layer.objCount = 0;
    }

    for (auto* obj : CCArrayExt<GameObject*>(lel->m_objects)) {
        layers[obj->m_editorLayer].objCount++;
        if (obj->m_editorLayer2 != obj->m_editorLayer && obj->m_editorLayer2 > 0) {
            layers[obj->m_editorLayer2].objCount++;
        }
    }

    requiresUpdate = false;
}

template <>
struct matjson::Serialize<ErGui::LayerInfo> {
    static Result<ErGui::LayerInfo> fromJson(const matjson::Value& value) {
        ErGui::LayerInfo ret;

        auto id = value["id"];
        if (!id.isExactlyUInt()) return Err("");
        ret.saveId = id.asUInt().unwrap();
        if (ret.saveId < 0 || ret.saveId > 10000) return Err("");

        ret.name = value["name"].asString().unwrapOr("");
        ret.isHidden = value["isHidden"].asBool().unwrapOr(false);
        ret.opacity = std::clamp((int)value["opacity"].asUInt().unwrapOr(0), 0, 255);

        return Ok(ret);
    }

    static matjson::Value toJson(const ErGui::LayerInfo& layerInfo) {
        return matjson::makeObject({
            { "id", layerInfo.saveId },
            { "name", layerInfo.name },
            { "opacity", layerInfo.opacity },
            { "isHidden", layerInfo.isHidden }
        });
    }
};


std::map<int, int> collectKey20Values(const std::string& s) {
    std::map<int, int> result;

    std::stringstream ss(s);
    std::string obj;

    while (std::getline(ss, obj, ';')) {
        if (obj.empty()) continue;

        std::stringstream os(obj);
        std::string key, value;

        int foundValue = -1;

        while (std::getline(os, key, ',') && std::getline(os, value, ',')) {
            if (key == "20") {
                auto let = numFromString<int>(value);
                if (let.isOk())
                    foundValue = let.unwrap();
                break;
            }
        }

        if (foundValue != -1) {
            result[foundValue]++;
        }
        else {
            result[0]++;
        }
    }

    return result;
}


class $modify(EditorUI) {
    void onCreateObject(int p0) {
        auto objString = GameManager::sharedState()->stringForCustomObject(p0);
        //log::info("{}", objString); // 20 key - editorL1 / 61 key - editorL2
        auto editorL1s = collectKey20Values(objString);
        for (auto pair : editorL1s) {
            log::info("{} - {}", pair.first, pair.second);
            ErGui::LAYER_STATE.layers[pair.first].objCount -= pair.second;
        }

        EditorUI::onCreateObject(p0);

        auto objs = EditorUI::get()->m_selectedObjects;
        auto obj = EditorUI::get()->m_selectedObject;

        if (obj) {
            ErGui::LAYER_STATE.layers[obj->m_editorLayer].objCount++;
        }
        else if (objs->count() > 0) {
            for (auto objInArr : CCArrayExt<GameObject*>(objs)) {
                ErGui::LAYER_STATE.layers[objInArr->m_editorLayer].objCount++;
            }
        }
    }
};

class $modify(LevelEditorLayer) {
    struct Fields {
        int levelId = 0;
        ~Fields() {
            ErGui::LAYER_STATE.clearUnused();
            std::vector<ErGui::LayerInfo> values;
            for (auto& kv : ErGui::LAYER_STATE.layers) {
                kv.second.saveId = kv.first;
                values.push_back(kv.second);
            }
            Mod::get()->setSavedValue<matjson::Value>(fmt::format("layers_{}", levelId), values);
            ErGui::LAYER_STATE.layers.clear();
            // log::debug("layers saved");
        }
    };

    void addSpecial(GameObject* obj) {
        ErGui::LAYER_STATE.layers[obj->m_editorLayer].objCount++;
        if (obj->m_editorLayer2 != obj->m_editorLayer && obj->m_editorLayer2 > 0) {
            ErGui::LAYER_STATE.layers[obj->m_editorLayer2].objCount++;
        }

        LevelEditorLayer::addSpecial(obj);
    }

    void removeSpecial(GameObject* obj) {
        ErGui::LAYER_STATE.layers[obj->m_editorLayer].objCount--;
        if (obj->m_editorLayer2 != obj->m_editorLayer && obj->m_editorLayer2 > 0) {
            ErGui::LAYER_STATE.layers[obj->m_editorLayer2].objCount--;
        }

        LevelEditorLayer::removeSpecial(obj);
    }

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;
        m_fields->levelId = EditorIDs::getID(p0);
        ErGui::LAYER_STATE.layers.clear();
        ErGui::LAYER_STATE.requiresUpdate = true;

        auto value = Mod::get()->getSavedValue<matjson::Value>(fmt::format("layers_{}", m_fields->levelId));
        auto layers = value.as<std::vector<ErGui::LayerInfo>>();
        if (layers.isOk()) {
            for (auto &&layer : *layers) {
                ErGui::LAYER_STATE.layers.insert({layer.saveId, layer});
            }
        }
        return true;
    }

    void updateObjectColors(CCArray* objs) {
        if (objs && objs->count() > 0) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(0));
            log::info("{} {} {}", obj->getColor().r, obj->getColor().g, obj->getColor().b);
            LevelEditorLayer::updateObjectColors(objs);
            log::info("{} {} {}", obj->getColor().r, obj->getColor().g, obj->getColor().b);
        }
        else {
            LevelEditorLayer::updateObjectColors(objs);
        }
    }

    // Could potentionally wake some visual bugs
    void updateVisibility(float p0) {
        LevelEditorLayer::updateVisibility(p0);
        // log::debug("{}--{}", m_activeObjectsCount, m_activeObjects.size()); different
        auto mode = this->m_playbackMode;
        if (mode == PlaybackMode::Playing) return;

        for (int i = 0; i < m_activeObjectsCount; i++) {
            GameObject* obj = m_activeObjects[i];
            if (ErGui::LAYER_STATE.layers[obj->m_editorLayer].isHidden || (obj->m_isHide && !obj->m_isSelected && GameManager::sharedState()->getGameVariable("0121"))) {
                obj->setOpacity(0);
            } else {
                bool isOnCurrentLayer = m_currentLayer == -1 || 
                    m_currentLayer == obj->m_editorLayer ||
                    m_currentLayer == obj->m_editorLayer2 && obj->m_editorLayer2 > 0;
    
                if (isOnCurrentLayer) {
                    obj->setOpacity(255);
                } else {
                    obj->setOpacity(ErGui::LAYER_STATE.layers[obj->m_editorLayer].opacity);
                }
            }

            // Hovering
            if (geode::Mod::get()->getSavedValue<bool>("hovering-selects", true)) {
                auto gearObj = static_cast<ErGui::GearGameObject*>(obj);
                auto isHovered = gearObj->m_fields->m_isHovered;
                if (isHovered) {
                    gearObj->m_fields->m_wasHovered = true;
                    gearObj->setObjectColor(ErGui::g_hoveredObjectColor);
                    if (gearObj->m_colorSprite)
                        gearObj->m_colorSprite->setColor(ErGui::g_hoveredObjectColor);
                }
                else if (gearObj->m_fields->m_wasHovered && gearObj->m_isSelected) {
                    gearObj->m_fields->m_wasHovered = false;
                    gearObj->setObjectColor(ErGui::g_selectedObjectColor);
                    if (gearObj->m_colorSprite)
                        gearObj->m_colorSprite->setColor(ErGui::g_selectedObjectColor);
                }
            }
        }
    }
};



void ErGui::renderLayerModule() {
    if (!ImGui::Begin("Layers")) {
        ImGui::End();
        LAYER_STATE.requiresUpdate = true;
        return;
    }

    ImGui::PushID("LayerModule");

    if (LAYER_STATE.requiresUpdate) LAYER_STATE.update();

    auto lel = LevelEditorLayer::get();

    // --------------------- header --------------------- 

    ImGui::SetNextItemWidth(100.f);
    int step = 1;
    if (ImGui::InputScalar("##LayerInput", ImGuiDataType_S16, &lel->m_currentLayer, &step)) {
        if (lel->m_currentLayer < -1) lel->m_currentLayer = -1;
    }
    ImGui::SameLine();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-1,5));
    if (ImGui::Button(ICON_MDI_MENU_LEFT_OUTLINE, ImVec2(21, 21))) {
        lel->m_currentLayer = -1;
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip("All Layer");
    ImGui::SameLine();

    if (ImGui::Button(ICON_MDI_MENU_RIGHT_OUTLINE, ImVec2(21, 21))) {
        ErGui::nextFreeLayer();
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip("Next Free Layer");

    ImGui::PopStyleVar();
    //ImGui::SameLine();

    //if (ImGui::Button("Refresh")) {
    //    LAYER_STATE.requiresUpdate = true;
    //}
    //ImGui::SameLine();

    if (ImGui::Button("Reset Opacity", ImVec2(100.f, 21.f))) {
        for (auto& layer : LAYER_STATE.layers) {
            layer.second.opacity = DEFAULT_OPACITY;
        }
    }

    ImGui::SameLine();
    auto editorLayer = lel->m_currentLayer;
    if (ImGui::BeginPopup("Confirm Delete")) {
        ImGui::Text("Are you sure, you want to \ndelete current layer?");
        if (ImGui::Button("No")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Yes")) {
            ImGui::CloseCurrentPopup();
            CCArray* toSelect = CCArray::create();
            for (auto* obj : CCArrayExt<GameObject*>(lel->m_objects)) {
                auto objectLayer1 = obj->m_editorLayer;
                auto objectLayer2 = obj->m_editorLayer2;
                if (objectLayer1 == editorLayer && objectLayer2 != editorLayer && objectLayer2 != 0) {
                    obj->m_editorLayer = objectLayer2;
                }
                else if (objectLayer1 != editorLayer && objectLayer2 == editorLayer) {
                    obj->m_editorLayer2 = objectLayer1;
                }
                else if (objectLayer1 == editorLayer || objectLayer2 == editorLayer) {
                    toSelect->addObject(obj);
                }
            }
            lel->m_editorUI->deselectAll();
            lel->m_editorUI->selectObjects(toSelect, false);
            lel->m_editorUI->onDeleteSelected(nullptr);
            LAYER_STATE.layers[editorLayer].objCount = 0;
        }
        ImGui::EndPopup();
    }

    ImGui::BeginDisabled(editorLayer == -1);
    if (ImGui::Button("Delete", ImVec2(42.f + ImGui::GetStyle().ItemSpacing.x, 21.f))) {
        ImGui::OpenPopup("Confirm Delete");
    }
    ImGui::EndDisabled();


    // --------------------- layers list ---------------------  
    ImVec2 scrollAvail = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("ScrollList", ImVec2(0, scrollAvail.y), true, 0);

    if (lel->m_currentLayer >= 0) {
        ErGui::LAYER_STATE.layers.insert({lel->m_currentLayer, LayerInfo()});
    }
    
    //ImGui::GetFont()->Scale = 24.f;
    int layerId = -1;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 14.f, 8.f });
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 1.f });

    if (ImGui::BeginTable("Layers", 4, ImGuiTableFlags_BordersInnerH)) {
        ImGui::TableSetupColumn("Visible", ImGuiTableColumnFlags_WidthFixed, BTN_SIZE.x + BTN_SIZE.x / 2.f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Opacity", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Locked", ImGuiTableColumnFlags_WidthFixed, BTN_SIZE.x + BTN_SIZE.x / 2.f);

        

        for (int i = 0; i < LAYER_STATE.layers.size(); ++i) {
            do { layerId++; } while (!LAYER_STATE.layers.contains(layerId));
            auto& layer = LAYER_STATE.layers.at(layerId);

            if (layer.name.empty() && layer.objCount == 0 && layerId != lel->m_currentLayer) {
                continue;
            }
            ImGui::PushID(layerId);

            float rowH = ImGui::GetFrameHeightWithSpacing();
            ImGui::TableNextRow(ImGuiTableRowFlags_None, rowH);

            
            // number
            ImGui::TableSetColumnIndex(1);
            ImGui::SameLine();
            ImGui::Text("%d. ", layerId);
            ImGui::SameLine();

            // layer name
            if (layer.isEditingName) {
                char buffer[128];
                strncpy(buffer, layer.name.c_str(), sizeof(buffer));
                buffer[sizeof(buffer) - 1] = '\0';
                // ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

                ImGui::SetNextItemWidth(std::max(60.f, ImGui::GetContentRegionAvail().x - 250));
                if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                    layer.name = std::string(buffer);
                }
                if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
                    layer.isEditingName = false;
                }
            }
            else {
                if (layer.name.empty()) {
                    ImGui::Text("Layer %d", layerId);
                }
                else {
                    ImGui::Text("%s", layer.name.c_str());
                }

                // double click - change
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                    layer.isEditingName = true;
                }
            }
            ImGui::SameLine();

            // obj count
            ImGui::Text("(Obj: %d)", layer.objCount);


            ImGui::TableSetColumnIndex(0);
            ImGui::SameLine(0.f, 0.f);
            bool selected = (layerId == lel->m_currentLayer);
            if (ImGui::Selectable("##row_select", &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_AllowDoubleClick , ImVec2(0, rowH))) {
                lel->m_currentLayer = layerId;
            }

            ImGui::SameLine();
            auto oldSize = ImGui::GetFont()->FontSize;
            ImGui::GetFont()->FontSize = oldSize * 24.f / 15.f;
            ImGui::AlignTextToFramePadding();
            if (layer.isHidden) {
                lel->m_lockedLayers[layerId] = true; // keep locked
                if (ImGui::Selectable(ICON_MDI_EYE_OFF, false, 0, BTN_SIZE)) {
                    layer.isHidden = false;
                    lel->m_lockedLayers[layerId] = layer.isLocked; // update locked state
                }
            }
            else {
                if (ImGui::Selectable(ICON_MDI_EYE, false, 0, BTN_SIZE)) {
                    layer.isHidden = true;
                }
            }

            ImGui::GetFont()->FontSize = oldSize;

            //ImGui::SameLine();
            ImGui::TableSetColumnIndex(2);

            auto posX = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(std::max(posX, posX + ImGui::GetContentRegionAvail().x - 140));

            // opacity slider
            ImGui::SetNextItemWidth(40);
            ImGui::DragInt("Opacity  ", &layer.opacity, 1, 0, 255);

            //ImGui::SameLine();
            ImGui::TableSetColumnIndex(3);
            // lock button
            ImGui::GetFont()->FontSize = oldSize * 24.f / 15.f;
            ImGui::AlignTextToFramePadding();
            if (layer.isLocked) {
                if (ImGui::Selectable(ICON_MDI_LOCK, false, 0, BTN_SIZE)) {
                    layer.isLocked = false;
                    if (!layer.isHidden) {
                        lel->m_lockedLayers[layerId] = false;
                    }
                }
            }
            else {
                if (ImGui::Selectable(ICON_MDI_LOCK_OPEN_OUTLINE, false, 0, BTN_SIZE)) {
                    layer.isLocked = true;
                    lel->m_lockedLayers[layerId] = true;
                }
            }
            ImGui::GetFont()->FontSize = oldSize;

            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleVar(2);

    ImGui::EndChild(); // scroll list

    ImGui::PopID();
    ImGui::End();
}



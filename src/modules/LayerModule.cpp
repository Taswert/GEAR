#pragma once
#include "LayerModule.hpp"
#include <imgui.h>
#include <algorithm>
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

#define DEFAULT_OPACITY 50


struct LayerInfo {
    uint16_t saveId;
    std::string name;
    int opacity = DEFAULT_OPACITY;
    ImTextureID image = 0;
    bool isEditingName = false;
    int objCount = 0;
};

template <>
struct matjson::Serialize<LayerInfo> {
    static Result<LayerInfo> fromJson(const matjson::Value& value) {
        LayerInfo ret;

        auto id = value["id"];
        if (!id.isExactlyUInt()) return Err("");
        ret.saveId = id.asUInt().unwrap();
        if (ret.saveId < 0 || ret.saveId > 10000) return Err("");

        ret.name = value["name"].asString().unwrapOr("");
        ret.opacity = std::clamp((int)value["opacity"].asUInt().unwrapOr(0), 0, 255);

        return Ok(ret);
    }

    static matjson::Value toJson(const LayerInfo& layerInfo) {
        return matjson::makeObject({
            { "id", layerInfo.saveId },
            { "name", layerInfo.name },
            { "opacity", layerInfo.opacity }
        });
    }
};

struct {
    std::unordered_map<int, LayerInfo> layers;
    bool requiresUpdate = false;

    void update() {
        auto lel = LevelEditorLayer::get();
        
        for (auto &[id, layer] : layers) {
            layer.objCount = 0;
        }

        for (auto *obj : CCArrayExt<GameObject*>(lel->m_objects)) {
            layers[obj->m_editorLayer].objCount++;
            if (obj->m_editorLayer2 != obj->m_editorLayer && obj->m_editorLayer2 > 0) {
                layers[obj->m_editorLayer2].objCount++;
            }
        }

        requiresUpdate = false;
    }

    void clearUnused() {
        // for (auto it = layers.begin(); it != layers.end();) {
        //     if (it->second.name.empty() && it->second.opacity == DEFAULT_OPACITY) {
        //         it = layers.erase(it);
        //     } else it++;
        // }
    }
} LAYER_STATE;



class $modify(LevelEditorLayer) {
    struct Fields {
        int levelId = 0;
        ~Fields() {
            LAYER_STATE.clearUnused();
            std::vector<LayerInfo> values;
            for (auto& kv : LAYER_STATE.layers) {
                kv.second.saveId = kv.first;
                values.push_back(kv.second);
            }
            Mod::get()->setSavedValue<matjson::Value>(fmt::format("layers_{}", levelId), values);
            LAYER_STATE.layers.clear();
            log::debug("layers saved");
        }
    };

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;
        m_fields->levelId = EditorIDs::getID(p0);
        LAYER_STATE.layers.clear();
        LAYER_STATE.requiresUpdate = true;

        auto value = Mod::get()->getSavedValue<matjson::Value>(fmt::format("layers_{}", m_fields->levelId));
        auto layers = value.as<std::vector<LayerInfo>>();
        if (layers.isOk()) {
            for (auto &&layer : *layers) {
                LAYER_STATE.layers.insert({layer.saveId, layer});
            }
        }
        return true;
    }

    void updateVisibility(float p0) {
        LevelEditorLayer::updateVisibility(p0);
        // log::debug("{}--{}", m_activeObjectsCount, m_activeObjects.size()); different
        for (int i = 0; i < m_activeObjectsCount; i++) {
            GameObject* obj = m_activeObjects[i];
            bool isOnCurrentLayer = m_currentLayer == -1 || 
                m_currentLayer == obj->m_editorLayer ||
                m_currentLayer == obj->m_editorLayer2 && obj->m_editorLayer2 > 0;

            if (isOnCurrentLayer) {
                obj->setOpacity(255);
            } else {
                obj->setOpacity(LAYER_STATE.layers[obj->m_editorLayer].opacity);
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

    ImGui::SetNextItemWidth(100);
    int step = 1;
    if (ImGui::InputScalar("##LayerInput", ImGuiDataType_S16, &lel->m_currentLayer, &step)) {
        if (lel->m_currentLayer < -1) lel->m_currentLayer = -1;
    }
    ImGui::SameLine();

    if (ImGui::Button("All")) {
        lel->m_currentLayer = -1;
    }
    ImGui::SameLine();

    if (ImGui::Button("Next Free")) {
        std::set<int> layersSet;
        for (auto obj : CCArrayExt<GameObject*>(lel->m_objects)) {
            if (obj->m_editorLayer >= 0)
                layersSet.insert(obj->m_editorLayer);
            if (obj->m_editorLayer2 > 0)
                layersSet.insert(obj->m_editorLayer2);
        }
        for (int i = 0; i < 10000; i++) {
            if (!layersSet.contains(i)) {
                lel->m_currentLayer = i;
                break;
            }
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Refresh")) {
        LAYER_STATE.requiresUpdate = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Reset Opacity")) {
        for (auto& layer : LAYER_STATE.layers) {
            layer.second.opacity = DEFAULT_OPACITY;
        }
    }


    // --------------------- layers list ---------------------  

    ImVec2 scrollAvail = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("ScrollList", ImVec2(0, scrollAvail.y), true, 0);

    if (lel->m_currentLayer >= 0) {
        LAYER_STATE.layers.insert({lel->m_currentLayer, LayerInfo()});
    }
    
    int layerId = -1;
    for (int i = 0; i < LAYER_STATE.layers.size(); ++i) {
        do {layerId++;} while (!LAYER_STATE.layers.contains(layerId));
        auto &layer = LAYER_STATE.layers.at(layerId);

        if (layer.name.empty() && layer.objCount == 0 && layer.image == 0 && layerId != lel->m_currentLayer) {
            continue;
        }

        ImGui::PushID(layerId);

        ImGui::BeginGroup();

        // number
        ImGui::Text(" %d.", layerId);
        ImGui::SameLine();

        // image
        ImVec2 imageSize = ImVec2(64, 64);
        if (layer.image)
            ImGui::Image(layer.image, imageSize);
        else
            ImGui::Dummy(imageSize);
        
        ImGui::SameLine();  

        // все что справа от картинки
        ImGui::BeginGroup();

        if (layer.isEditingName) {
            char buffer[128];
            strncpy(buffer, layer.name.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer))) {
                layer.name = std::string(buffer);
            }
            if (!ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
                layer.isEditingName = false;
            }
        } else {
            if (layer.name.empty()) {
                ImGui::Text("Layer %d", layerId);
            } else {
                ImGui::Text("%s", layer.name.c_str());
            }

            // ПКМ - изменить
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                layer.isEditingName = true;
            }

            ImGui::SameLine();
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 1));
        }

        // lock button
        bool isLocked = lel->m_lockedLayers[layerId];
        if (ImGui::Selectable("[Lock]", &isLocked, 0, ImVec2(45, 15))) {
            lel->m_lockedLayers[layerId] = !lel->m_lockedLayers[layerId];
        }
        ImGui::SameLine();

        ImGui::Text("Obj: %d", layer.objCount);

        // opacity slider
        ImGui::SetNextItemWidth(50);
        if (ImGui::DragInt("Opacity", &layer.opacity, 1, 0, 255)) {

        }

        ImGui::EndGroup();

        ImGui::Separator();

        ImGui::EndGroup();
        
        ImVec2 groupMax = ImGui::GetItemRectMax();
        ImVec2 groupMin = ImGui::GetItemRectMin();

        // if current layer
        if (layerId == lel->m_currentLayer) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(groupMin, groupMax, IM_COL32(155, 155, 155, 50), 0);
        }

        // hover
        if (ImGui::IsItemHovered()) {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(groupMin, groupMax, IM_COL32(155, 155, 155, 20), 0);
        }

        // on click
        if (!ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive() && ImGui::IsItemClicked()) {
            lel->m_currentLayer = layerId;
        }

        ImGui::PopID();
    }


    ImGui::EndChild(); // scroll list

    ImGui::PopID();
    ImGui::End();
}



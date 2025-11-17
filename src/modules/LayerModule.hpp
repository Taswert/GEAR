#pragma once

#define DEFAULT_OPACITY 50
#define BTN_SIZE ImVec2(16,20)

namespace ErGui {
	void renderLayerModule();

    struct LayerInfo {
        uint16_t saveId;
        std::string name;
        int opacity = DEFAULT_OPACITY;
        bool isHidden = false;
        bool isLocked = false;
        bool isEditingName = false;
        int objCount = 0;
    };

    struct LayerState {
        std::unordered_map<int, LayerInfo> layers;
        bool requiresUpdate = false;

        void update();

        void clearUnused() {
            // for (auto it = layers.begin(); it != layers.end();) {
            //     if (it->second.name.empty() && it->second.opacity == DEFAULT_OPACITY) {
            //         it = layers.erase(it);
            //     } else it++;
            // }
        }
    } inline LAYER_STATE;
}
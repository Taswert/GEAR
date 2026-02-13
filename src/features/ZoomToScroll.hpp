#pragma once
#include "../classes/GearEditorUI.hpp"
namespace ZoomToScroll {
    bool scrollWheel(GearEditorUI* self, float p0, float p1);
    void update(GearEditorUI* self, float dt);
}
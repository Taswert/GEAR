#pragma once
#include <imgui.h>
#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include "myUtils.hpp"
using namespace geode::prelude;

namespace ErGui {
	void clampHSV(ccHSVValue* hsvValue);
	bool colorSelectImGuiPopup(int* colorId, std::string popupStr, bool colorEditRestriction, EffectGameObject* objWithLabel = nullptr);
	void renderEditColor();
}
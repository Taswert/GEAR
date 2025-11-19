#include "EnterTriggers.hpp"
using namespace ErGui;

void drawEnterEffectSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EnterEffectObject*>(obj);

	bool enterOnly = eObj->m_enterType == 1;
	bool exitOnly = eObj->m_enterType == 2;

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Target Enter Channel", reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(eObj) + 0x800));

	if (ImGui::Checkbox("Enter Only", &enterOnly)) {
		exitOnly = false;
		if (enterOnly) eObj->m_enterType = 1;
		else eObj->m_enterType = 0;
	}
	if (ImGui::Checkbox("Exit Only", &exitOnly)) {
		enterOnly = false;
		if (exitOnly) eObj->m_enterType = 2;
		else eObj->m_enterType = 0;
	}
	// ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	// ImGui::Combo("Enter Type", &eObj->m_enterType, enterType, IM_ARRAYSIZE(enterType));

	drawTouchSpawnTriggered(eObj, objArr);
}

void ErGui::mapEnterTriggers() {
	triggersMap[22] = drawEnterEffectSettings;
	triggersMap[23] = drawEnterEffectSettings;
	triggersMap[24] = drawEnterEffectSettings;
	triggersMap[25] = drawEnterEffectSettings;
	triggersMap[26] = drawEnterEffectSettings;
	triggersMap[27] = drawEnterEffectSettings;
	triggersMap[28] = drawEnterEffectSettings;
	triggersMap[55] = drawEnterEffectSettings;
	triggersMap[56] = drawEnterEffectSettings;
	triggersMap[57] = drawEnterEffectSettings;
	triggersMap[58] = drawEnterEffectSettings;
	triggersMap[59] = drawEnterEffectSettings;
	triggersMap[1915] = drawEnterEffectSettings;
}
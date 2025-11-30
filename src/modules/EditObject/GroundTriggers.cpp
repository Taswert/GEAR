#include "GroundTriggers.hpp"
using namespace ErGui;

void drawSetupMGSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	float yOffset = eObj->m_moveOffset.y / 3.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Offset Y", &yOffset, 1.f, -100.f, 100.f, "%.0f")) {
		eObj->m_moveOffset.y = yOffset * 3.f;
	}

	drawEasingSettings(eObj, objArr);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Move Time", &eObj->m_duration, .05f, 0.f, 10.f)) {
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawBGMGSpeedSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Mod X", &eObj->m_moveModX, .05f, 0.f, 1.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Mod Y", &eObj->m_moveModY, .05f, 0.f, 1.f);

	drawTouchSpawnTriggered(eObj, objArr);
}

void ErGui::mapGroundTriggers() {
	//BG Change
	//G Change
	//MG Change
	triggersMap[2999] = drawSetupMGSettings;
	triggersMap[3606] = drawBGMGSpeedSettings;
	triggersMap[3612] = drawBGMGSpeedSettings;
}
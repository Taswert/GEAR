#include "FollowTriggers.hpp"
using namespace ErGui;

void drawFollowSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("X Mod", &eObj->m_followXMod, .05f, -1.f, 1.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Y Mod", &eObj->m_followYMod, .05f, -1.f, 1.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Move Time", &eObj->m_duration, 0.1f, 0.5f, "%.3f")) {
		if (eObj->m_duration < -1.f) eObj->m_duration = -1.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Follow Group ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
	}

	drawTouchSpawnTriggered(eObj);
}

void drawFollowPlayerY(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EnhancedTriggerObject*>(obj);


	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Speed", &eObj->m_followYSpeed, 1.f, 10.f, "%.3f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Delay", &eObj->m_followYDelay, 1.f, 10.f, "%.3f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Offset", &eObj->m_followYOffset, 1.f, 10.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Max Speed", &eObj->m_followYMaxSpeed, 1.f, 10.f, "%.3f");

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Move Time", &eObj->m_duration, 0.1f, 0.5f, "%.3f")) {
		if (eObj->m_duration < -1.f) eObj->m_duration = -1.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(eObj);
}


void ErGui::mapFollowTriggers() {
	triggersMap[1347] = drawFollowSettings;
	triggersMap[1814] = drawFollowPlayerY;
	//Advanced Follow
	//Edit Advanced Follow
	//ReTarget Advanced Follow
}
#include "CollisionTriggers.hpp"
using namespace ErGui;

void drawCollisionTriggerSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::Checkbox("P1", &eObj->m_targetPlayer1);
	ImGui::SameLine();
	ImGui::Checkbox("P2", &eObj->m_targetPlayer2);
	ImGui::SameLine();
	ImGui::Checkbox("PP", &eObj->m_followCPP);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockA ID", &eObj->m_itemID)) {
		if (eObj->m_itemID < 0) eObj->m_itemID = 0;
		if (eObj->m_itemID > 9999) eObj->m_itemID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockB ID", &eObj->m_itemID2)) {
		if (eObj->m_itemID2 < 0) eObj->m_itemID2 = 0;
		if (eObj->m_itemID2 > 9999) eObj->m_itemID2 = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::Checkbox("Activate Group", &eObj->m_activateGroup);
	ImGui::Checkbox("Trigger On Exit", &eObj->m_triggerOnExit);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawInstantCollisionSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockA ID", &eObj->m_itemID)) {
		if (eObj->m_itemID < 0) eObj->m_itemID = 0;
		if (eObj->m_itemID > 9999) eObj->m_itemID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockB ID", &eObj->m_itemID2)) {
		if (eObj->m_itemID2 < 0) eObj->m_itemID2 = 0;
		if (eObj->m_itemID2 > 9999) eObj->m_itemID2 = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("True ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("False ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
	}

	ImGui::Checkbox("P1", &eObj->m_targetPlayer1);
	ImGui::SameLine();
	ImGui::Checkbox("P2", &eObj->m_targetPlayer2);
	ImGui::SameLine();
	ImGui::Checkbox("PP", &eObj->m_followCPP);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawCollisionStateSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("State On", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("State Off", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
}

void drawCollisionBlockSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Block ID", &eObj->m_itemID)) {
		if (eObj->m_itemID < 0) eObj->m_itemID = 0;
		if (eObj->m_itemID > 9999) eObj->m_itemID = 9999;
		lel->updateObjectLabel(obj);
	}

	if (ImGui::Checkbox("Dynamic Block", &eObj->m_isDynamicBlock)) {
		lel->updateObjectLabel(obj);
	}
}

void drawToggleBlockSettings(GameObject* obj, CCArray* objArr) {
	auto rObj = static_cast<RingObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &rObj->m_targetGroupID)) {
		if (rObj->m_targetGroupID < 0) rObj->m_targetGroupID = 0;
		if (rObj->m_targetGroupID > 9999) rObj->m_targetGroupID = 9999;
	}

	ImGui::Checkbox("Activate Group", &rObj->m_activateGroup);
	ImGui::Checkbox("Claim Touch", &rObj->m_claimTouch);
	ImGui::Checkbox("Multi Activate", &rObj->m_isMultiActivate);
	ImGui::Checkbox("Spawn Only", &rObj->m_isSpawnOnly);
}

void ErGui::mapCollisionTriggers() {
	triggersMap[1815] = drawCollisionTriggerSettings;
	triggersMap[3609] = drawInstantCollisionSettings;
	triggersMap[3640] = drawCollisionStateSettings;
	triggersMap[1816] = drawCollisionBlockSettings;
	triggersMap[3643] = drawToggleBlockSettings;
}
#include "CollisionTriggers.hpp"
using namespace ErGui;

void drawCollisionTriggerSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::Checkbox("P1", &cObj->m_targetPlayer1);
	ImGui::SameLine();
	ImGui::Checkbox("P2", &cObj->m_targetPlayer2);
	ImGui::SameLine();
	ImGui::Checkbox("PP", &cObj->m_followCPP);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockA ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockB ID", &cObj->m_itemID2)) {
		if (cObj->m_itemID2 < 0) cObj->m_itemID2 = 0;
		if (cObj->m_itemID2 > 9999) cObj->m_itemID2 = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::Checkbox("Activate Group", &cObj->m_activateGroup);
	ImGui::Checkbox("Trigger On Exit", &cObj->m_triggerOnExit);

	drawTouchSpawnTriggered(cObj);
}

void drawInstantCollisionSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockA ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("BlockB ID", &cObj->m_itemID2)) {
		if (cObj->m_itemID2 < 0) cObj->m_itemID2 = 0;
		if (cObj->m_itemID2 > 9999) cObj->m_itemID2 = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("True ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("False ID", &cObj->m_centerGroupID)) {
		if (cObj->m_centerGroupID < 0) cObj->m_centerGroupID = 0;
		if (cObj->m_centerGroupID > 9999) cObj->m_centerGroupID = 9999;
	}

	ImGui::Checkbox("P1", &cObj->m_targetPlayer1);
	ImGui::SameLine();
	ImGui::Checkbox("P2", &cObj->m_targetPlayer2);
	ImGui::SameLine();
	ImGui::Checkbox("PP", &cObj->m_followCPP);

	drawTouchSpawnTriggered(cObj);
}

void drawCollisionStateSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("State On", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("State Off", &cObj->m_centerGroupID)) {
		if (cObj->m_centerGroupID < 0) cObj->m_centerGroupID = 0;
		if (cObj->m_centerGroupID > 9999) cObj->m_centerGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
}

void drawCollisionBlockSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Block ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
		lel->updateObjectLabel(obj);
	}

	if (ImGui::Checkbox("Dynamic Block", &cObj->m_isDynamicBlock)) {
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
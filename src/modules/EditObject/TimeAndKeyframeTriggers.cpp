#include "TimeAndKeyframeTriggers.hpp"
using namespace ErGui;

void drawTimeControlSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<TimerTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &eObj->m_itemID)) {
		if (eObj->m_itemID < 0) eObj->m_itemID = 0;
		if (eObj->m_itemID > 9999) eObj->m_itemID = 9999;
	}

	bool start = eObj->m_controlType == 0;
	bool stop = eObj->m_controlType == 1;

	if (ImGui::Checkbox("Start", &start)) {
		stop = false;
		eObj->m_controlType = 0;
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Stop", &stop)) {
		start = false;
		eObj->m_controlType = 1;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawKeyframeAnimationSettings(GameObject* obj, CCArray* objArr) {
	auto kObj = static_cast<KeyframeAnimTriggerObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Animation Group ID", &kObj->m_animationID);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &kObj->m_targetGroupID)) {
		if (kObj->m_targetGroupID < 0) kObj->m_targetGroupID = 0;
		if (kObj->m_targetGroupID > 9999) kObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Parent ID", &kObj->m_centerGroupID)) {
		if (kObj->m_centerGroupID < 0) kObj->m_centerGroupID = 0;
		if (kObj->m_centerGroupID > 9999) kObj->m_centerGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Time Mod", &kObj->m_timeMod, .1f, 0.f, 1.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Position X Mod", &kObj->m_positionXMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Position Y Mod", &kObj->m_positionYMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Rotation Mod", &kObj->m_rotationMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Scale X Mod", &kObj->m_scaleXMod, .1f, -2.f, 2.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Scale Y Mod", &kObj->m_scaleYMod, .1f, -2.f, 2.f);

	drawTouchSpawnTriggered(kObj, objArr);
}

void ErGui::mapTimeAndKeyframeTriggers() {
	// Time
	// Time Event
	triggersMap[3617] = drawTimeControlSettings;
	triggersMap[3033] = drawKeyframeAnimationSettings;
	// Setup Keyframe
}
#include "CountAndItemTriggers.hpp"
using namespace ErGui;

void drawCountSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CountTriggerGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Target Count", &cObj->m_pickupCount);

	ImGui::Checkbox("Activate Group", &cObj->m_activateGroup);
	ImGui::Checkbox("Multi Activate", &cObj->m_multiActivate);

	drawTouchSpawnTriggered(cObj);
}

void drawInstantCountSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CountTriggerGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Target Count", &cObj->m_pickupCount);

	ImGui::Checkbox("Activate Group", &cObj->m_activateGroup);

	bool equals = cObj->m_pickupTriggerMode == 0;
	bool larger = cObj->m_pickupTriggerMode == 1;
	bool smaller = cObj->m_pickupTriggerMode == 2;

	ImGui::Separator();

	if (ImGui::Checkbox("Equals", &equals)) {
		larger = false;
		smaller = false;
		cObj->m_pickupTriggerMode = 0;
	}
	if (ImGui::Checkbox("Larger", &larger)) {
		equals = false;
		smaller = false;
		cObj->m_pickupTriggerMode = 1;
	}
	if (ImGui::Checkbox("Smaller", &smaller)) {
		equals = false;
		larger = false;
		cObj->m_pickupTriggerMode = 2;
	}

	ImGui::Separator();

	drawTouchSpawnTriggered(cObj);
}

void drawPickupSettings(GameObject* obj, CCArray* objArr) {
	auto pObj = static_cast<CountTriggerGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &pObj->m_itemID)) {
		if (pObj->m_itemID < 0) pObj->m_itemID = 0;
		if (pObj->m_itemID > 9999) pObj->m_itemID = 9999;
		lel->updateObjectLabel(obj);
	}

	bool multiply = pObj->m_pickupTriggerMode == 1;
	bool divide = pObj->m_pickupTriggerMode == 2;

	if (ImGui::Checkbox("Multiply", &multiply)) {
		divide = false;
		if (multiply) pObj->m_pickupTriggerMode = 1;
		else pObj->m_pickupTriggerMode = 0;
	}

	if (ImGui::Checkbox("Divide", &divide)) {
		multiply = false;
		if (divide) pObj->m_pickupTriggerMode = 2;
		else pObj->m_pickupTriggerMode = 0;
	}

	ImGui::Separator();

	if (pObj->m_pickupTriggerMode == 0) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputInt("Count", &pObj->m_pickupCount);

		ImGui::Checkbox("Override", &pObj->m_isOverride);
	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Multiplier", &pObj->m_pickupTriggerMultiplier, .05f, 0.f, 10.f);
	}

	drawTouchSpawnTriggered(pObj);
}

void drawCounterLabelSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<LabelGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &cObj->m_itemID)) {
		if (cObj->m_itemID < 0) cObj->m_itemID = 0;
		if (cObj->m_itemID > 9999) cObj->m_itemID = 9999;
		cObj->updatePreviewLabel();
	}

	auto leftAlign = cObj->m_alignment == 1;
	auto rightAlign = cObj->m_alignment == 2;

	if (ImGui::Checkbox("Left Align", &leftAlign)) {
		rightAlign = false;
		if (leftAlign) cObj->m_alignment = 1;
		else cObj->m_alignment = 0;
		cObj->updateLabelAlign(cObj->m_alignment);
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Right Align", &rightAlign)) {
		leftAlign = false;
		if (rightAlign) cObj->m_alignment = 2;
		else cObj->m_alignment = 0;
		cObj->updateLabelAlign(cObj->m_alignment);
	}

	if (ImGui::Checkbox("Time Counter", &cObj->m_isTimeCounter))
		cObj->updatePreviewLabel();
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Seconds Only", &cObj->m_showSecondsOnly);

	bool mainTime = cObj->m_shownSpecial == -1;
	bool points = cObj->m_shownSpecial == -2;
	bool attempts = cObj->m_shownSpecial == -3;

	ImGui::Separator();

	if (ImGui::Checkbox("Main Time", &mainTime)) {
		points = false;
		attempts = false;
		if (mainTime) cObj->m_shownSpecial = -1;
		else cObj->m_shownSpecial = 0;
		cObj->updatePreviewLabel();
	}
	if (ImGui::Checkbox("Points", &points)) {
		mainTime = false;
		attempts = false;
		if (points) cObj->m_shownSpecial = -2;
		else cObj->m_shownSpecial = 0;
		cObj->updatePreviewLabel();
	}
	if (ImGui::Checkbox("Attempts", &attempts)) {
		points = false;
		mainTime = false;
		if (attempts) cObj->m_shownSpecial = -3;
		else cObj->m_shownSpecial = 0;
		cObj->updatePreviewLabel();
	}
}

void drawItemPersSettings(GameObject* obj, CCArray* objArr) {
	auto iObj = static_cast<ItemTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Item ID", &iObj->m_itemID)) {
		if (iObj->m_itemID < 0) iObj->m_itemID = 0;
		if (iObj->m_itemID > 9999) iObj->m_itemID = 9999;
	}

	ImGui::Checkbox("Timer", &iObj->m_timer);
	ImGui::Checkbox("Persistent", &iObj->m_persistent);
	ImGui::Checkbox("Target All", &iObj->m_targetAll);
	ImGui::Checkbox("Reset", &iObj->m_reset);

	drawTouchSpawnTriggered(iObj);
}

void ErGui::mapCountAndItemTriggers() {
	// Count
	triggersMap[1611] = drawCountSettings;
	triggersMap[1811] = drawInstantCountSettings;
	triggersMap[1817] = drawPickupSettings;
	triggersMap[1615] = drawCounterLabelSettings;

	// Item
	triggersMap[3641] = drawItemPersSettings;
	//Item Compare
	//Item Edit
}
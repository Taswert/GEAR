#include "CameraTriggers.hpp"
using namespace ErGui;

void drawCameraZoomSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Zoom", &eObj->m_zoomValue, .05f, .4f, 3.f);

	drawEasingSettings(eObj, objArr, ErGui::INPUT_ITEM_WIDTH);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Move Time", &eObj->m_duration, .05f, 0.f, 10.f)) {
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawCameraStaticSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<CameraTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Group ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9;
	}

	ImGui::Checkbox("Exit Static", &eObj->m_exitStatic);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Exit Instant", &eObj->m_exitInstant);

	bool xOnly = eObj->m_moveTargetMode == MoveTargetType::XOnly;
	bool yOnly = eObj->m_moveTargetMode == MoveTargetType::YOnly;

	if (ImGui::Checkbox("X Only", &xOnly)) {
		if (xOnly) eObj->m_moveTargetMode = MoveTargetType::XOnly;
		else eObj->m_moveTargetMode = MoveTargetType::Both;
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Y Only", &yOnly)) {
		if (yOnly) eObj->m_moveTargetMode = MoveTargetType::YOnly;
		else eObj->m_moveTargetMode = MoveTargetType::Both;
	}

	ImGui::Checkbox("Follow", &eObj->m_followObject);
	if (eObj->m_followObject) {
		ImGui::SameLine(150.f);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Easing", &eObj->m_followEasing, .5f, 0.f, 50.f);
	}

	ImGui::Checkbox("Smooth Velocity", &eObj->m_smoothVelocity);
	if (eObj->m_smoothVelocity) {
		ImGui::SameLine(150.f);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Modifier", &eObj->m_velocityModifier, .05f, 0.f, 1.f);
	}

	drawEasingSettings(eObj, objArr, ErGui::INPUT_ITEM_WIDTH);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Move Time", &eObj->m_duration, .05f, 0.f, 10.f)) {
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawCameraOffsetSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CameraTriggerGameObject*>(obj);

	float xOffset = cObj->m_moveOffset.x / 3.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Offset X", &xOffset, 1.f, -100.f, 100.f, "%.0f")) {
		cObj->m_moveOffset.x = xOffset * 3.f;
	}

	bool xOnly = cObj->m_moveTargetMode == MoveTargetType::XOnly;
	if (ImGui::Checkbox("X Only", &xOnly)) {
		if (xOnly) cObj->m_moveTargetMode = MoveTargetType::XOnly;
		else cObj->m_moveTargetMode = MoveTargetType::Both;
	}

	float yOffset = cObj->m_moveOffset.y / 3.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Offset Y", &yOffset, 1.f, -100.f, 100.f, "%.0f")) {
		cObj->m_moveOffset.y = yOffset * 3.f;
	}

	bool yOnly = cObj->m_moveTargetMode == MoveTargetType::YOnly;
	if (ImGui::Checkbox("Y Only", &yOnly)) {
		if (yOnly) cObj->m_moveTargetMode = MoveTargetType::YOnly;
		else cObj->m_moveTargetMode = MoveTargetType::Both;
	}

	drawEasingSettings(cObj, objArr, ErGui::INPUT_ITEM_WIDTH);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Move Time", &cObj->m_duration, .05f, 0.f, 10.f)) {
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		cObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(cObj, objArr);
}

void drawCameraGameplaySettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	if (ImGui::Button("Default")) {
		eObj->m_moveOffset = ccp(75.f, 75.f);
	}

	float xOffset = eObj->m_moveOffset.x / 3.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Offset X", &xOffset, 1.f, -100.f, 100.f, "%.0f")) {
		eObj->m_moveOffset.x = xOffset * 3.f;
	}

	bool xOnly = eObj->m_moveTargetMode == MoveTargetType::XOnly;
	if (ImGui::Checkbox("X Only", &xOnly)) {
		if (xOnly) eObj->m_moveTargetMode = MoveTargetType::XOnly;
		else eObj->m_moveTargetMode = MoveTargetType::Both;
	}
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Don't Zoom##X", &eObj->m_lockToPlayerX);

	float yOffset = eObj->m_moveOffset.y / 3.f;
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Offset Y", &yOffset, 1.f, -100.f, 100.f, "%.0f")) {
		eObj->m_moveOffset.y = yOffset * 3.f;
	}

	bool yOnly = eObj->m_moveTargetMode == MoveTargetType::YOnly;
	if (ImGui::Checkbox("Y Only", &yOnly)) {
		if (yOnly) eObj->m_moveTargetMode = MoveTargetType::YOnly;
		else eObj->m_moveTargetMode = MoveTargetType::Both;
	}
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Don't Zoom##Y", &eObj->m_lockToPlayerY);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawCameraRotateSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CameraTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Degrees", &cObj->m_rotationDegrees, .5f, -360.f, 360.f);

	ImGui::Checkbox("Snap 360", &cObj->m_isDirectionFollowSnap360);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Add", &cObj->m_lockObjectRotation);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Move Time", &cObj->m_duration, .05f, 0.f, 10.f)) {
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		cObj->m_endPosition = *somePoint;
	}

	drawEasingSettings(cObj, objArr, ErGui::INPUT_ITEM_WIDTH);

	drawTouchSpawnTriggered(cObj, objArr);
}

void drawCameraEdgeSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CameraTriggerGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::Combo("Edge Direction", &cObj->m_edgeDirection, edgeDirection, IM_ARRAYSIZE(edgeDirection));

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	if (ImGui::Button("Unlock")) {
		cObj->m_targetGroupID = 0; // this is literally what it does
		lel->updateObjectLabel(obj);
	}

	drawTouchSpawnTriggered(cObj, objArr);
}

void drawCameraModeSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CameraTriggerGameObject*>(obj);

	ImGui::Checkbox("Free Mode", &cObj->m_cameraIsFreeMode);

	ImGui::Checkbox("Edit Camera Settings", &cObj->m_cameraEditCameraSettings);
	if (cObj->m_cameraEditCameraSettings) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Easing", &cObj->m_cameraEasingValue, 1.f, 1.f, 40.f, "%.0f");

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Padding", &cObj->m_cameraPaddingValue, .05f, 0.f, 1.f, "%.2f");
	}

	ImGui::Checkbox("Disable Grid Snap", &cObj->m_cameraDisableGridSnap);

	drawTouchSpawnTriggered(cObj, objArr);
}

void drawCameraGuideSettings(GameObject* obj, CCArray* objArr) {
	auto cObj = static_cast<CameraTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Zoom", &cObj->m_zoomValue, .05f, .4f, 3.f);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Offset X", &cObj->m_moveOffset.x, 1.f, -100.f, 100.f, "%.0f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Offset Y", &cObj->m_moveOffset.y, 1.f, -100.f, 100.f, "%.0f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Preview Opacity", &cObj->m_previewOpacity, .05f, .0f, 1.f);
}

void ErGui::mapCameraTriggers() {
	triggersMap[1913] = drawCameraZoomSettings;
	triggersMap[1914] = drawCameraStaticSettings;
	triggersMap[1916] = drawCameraOffsetSettings;
	triggersMap[2901] = drawCameraGameplaySettings;
	triggersMap[2015] = drawCameraRotateSettings;
	triggersMap[2062] = drawCameraEdgeSettings;
	triggersMap[2925] = drawCameraModeSettings;
	triggersMap[2016] = drawCameraGuideSettings;
}
#include "StandardTriggers.hpp"
#include "EditColorModule.hpp"
#include "ObjectProperty.hpp"
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/EnhancedGameObject.hpp>

using namespace ErGui;

void drawStartPosSettings(GameObject* obj, CCArray* objArr) {
	auto spObj = static_cast<StartPosObject*>(obj);
	auto spSettings = spObj->m_startSettings;

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::Combo("Mode", &spSettings->m_startMode, gameModesItems, IM_ARRAYSIZE(gameModesItems));

	int speedIndex = 0;
	for (int i = 0; i < IM_ARRAYSIZE(speedValues); i++) {
		if ((int)spSettings->m_startSpeed == speedValues[i]) {
			speedIndex = i;
			break;
		}
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::Combo("Speed", &speedIndex, speedItems, IM_ARRAYSIZE(speedItems))) {
		(int&)spSettings->m_startSpeed = speedValues[speedIndex];
	}

	ImGui::Separator();

	ImGui::Checkbox("Flip", &spSettings->m_isFlipped);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Mini", &spSettings->m_startMini);

	ImGui::Checkbox("Dual", &spSettings->m_startDual);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Mirror", &spSettings->m_mirrorMode);

	ImGui::Checkbox("Rotate", &spSettings->m_rotateGameplay);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Reverse", &spSettings->m_reverseGameplay);

	ImGui::Checkbox("Reset Camera", &spSettings->m_resetCamera);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Disable", &spSettings->m_disableStartPos);

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Order", &spSettings->m_targetOrder)) {
		if (spSettings->m_targetOrder < 0) spSettings->m_targetOrder = 0;
		if (spSettings->m_targetOrder > 999999) spSettings->m_targetOrder = 999999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Channel", &spSettings->m_targetChannel)) {
		if (spSettings->m_targetChannel < 0) spSettings->m_targetChannel = 0;
		if (spSettings->m_targetChannel > 999999) spSettings->m_targetChannel = 999999;
	}
}

void drawColorSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	std::string btnStr = "##";
	float r, g, b;
	float hsv[4] = { savedHueEO, savedSaturationEO, savedValueEO, eObj->m_opacity };
	ImGuiColorEditFlags flags =
		ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_AlphaBar;
	if (geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel", true))
		flags |= ImGuiColorEditFlags_PickerHueWheel;
	if (!geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel", false))
		flags |= ImGuiColorEditFlags_DisableWheelRot;

	if (eObj->m_copyColorID == 0) {
		ImGui::ColorPicker4((btnStr + "COLOR-SETTINGS").c_str(), hsv, flags);

		savedHueEO = hsv[0];
		savedSaturationEO = hsv[1];
		savedValueEO = hsv[2];
		eObj->m_opacity = hsv[3];

		ImGui::ColorConvertHSVtoRGB(savedHueEO, savedSaturationEO, savedValueEO, r, g, b);
		eObj->m_triggerTargetColor = ccColor3B(int(r * 255), int(g * 255), int(b * 255));
	}
	else {
		ImGui::DragFloat("Hue", &eObj->m_hsvValue.h, 1.f, -180.f, 180.f);
		ImGui::DragFloat("Saturation", &eObj->m_hsvValue.s, 0.05f, -1.f, 2.f);
		ImGui::DragFloat("Value", &eObj->m_hsvValue.v, 0.05f, -1.f, 2.f);

		ImGui::Checkbox("Absolute Saturation##AS", &eObj->m_hsvValue.absoluteSaturation);
		ImGui::Checkbox("Absolute Value##AV", &eObj->m_hsvValue.absoluteBrightness);
		//ImGui::Checkbox("Legacy HSV##what", &ccMyColor->m_legacyHSV); TRIGGER FUNCTION

		ErGui::clampHSV(&eObj->m_hsvValue);

		if (ImGui::Button("HSV Reset##HSV-RESET")) {
			eObj->m_hsvValue.absoluteBrightness = false;
			eObj->m_hsvValue.absoluteSaturation = false;
			eObj->m_hsvValue.h = 0.f;
			eObj->m_hsvValue.s = 1.f;
			eObj->m_hsvValue.v = 1.f;
		}

		ImGui::Checkbox("Legacy HSV", &eObj->m_legacyHSV);

		ImGui::Checkbox("Copy Opacity", &eObj->m_copyOpacity);
		if (!eObj->m_copyOpacity)
			ImGui::DragFloat("Opacity", &eObj->m_opacity, 0.05f, 0.f, 1.f);

	}

	if (!isOldColorTrigger(eObj)) {
		auto targetColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(eObj->m_targetColor);
		if (ImGui::ColorButton(btnStr.c_str(), ImVec4(targetColorAction->m_fromColor.r / 255.f, targetColorAction->m_fromColor.g / 255.f, targetColorAction->m_fromColor.b / 255.f, 1.f))) {
			ImGui::OpenPopup("CSP##TARGET");
		}
		ErGui::colorSelectImGuiPopup(&eObj->m_targetColor, "CSP##TARGET", false, eObj);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.f);
		int targetID = eObj->m_targetColor;
		if (ImGui::InputInt(("Target Color##INPUT-TARGETCOLOR"), &targetID)) {
			if (targetID > 1101)					targetID = 1101;
			if (targetID < 0)						targetID = 0;

			eObj->m_targetColor = targetID;
			LevelEditorLayer::get()->updateObjectLabel(eObj);
		}
	}

	ImGui::Checkbox("Blending", &eObj->m_usesBlending);

	auto copyColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(eObj->m_copyColorID);
	if (ImGui::ColorButton(std::string(btnStr + "##COPY-COLOR").c_str(), ImVec4(copyColorAction->m_fromColor.r / 255.f, copyColorAction->m_fromColor.g / 255.f, copyColorAction->m_fromColor.b / 255.f, 1.f))) {
		ImGui::OpenPopup("CSP##COPY");
	}
	ErGui::colorSelectImGuiPopup(&eObj->m_copyColorID, "CSP##COPY", true);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(80.f);
	int copyID = eObj->m_copyColorID;
	if (ImGui::InputInt(("##INPUT-COPYCOLOR"), &copyID)) {
		if (copyID > 1101)					copyID = 1101;
		if (copyID < 0)						copyID = 0;

		eObj->m_copyColorID = copyID;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		eObj->m_copyColorID = 0;
	}
	ImGui::SameLine();
	ImGui::Text("Copy Color");

	if (eObj->m_objectID == 29) {
		ImGui::Checkbox("Tint Ground", &eObj->m_tintGround);
	}

	drawComponentTime(eObj, objArr, "Fade Time", false);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawMoveSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);


	ErGui::drawComponentGroupID(eObj, objArr, "Group ID");


	ImGui::SeparatorText("Core Settings");

	float modStep = 1.f;
	if (!eObj->m_smallStep) modStep = 3.f;

	if (!eObj->m_useMoveTarget && !eObj->m_isDirectionFollowSnap360) {
		float xStep = eObj->m_moveOffset.x / modStep;
		float yStep = eObj->m_moveOffset.y / modStep;

		
		if (!eObj->m_lockToCameraX && !eObj->m_lockToPlayerX) {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			
			// if (syncObjectCCPoint(objArr, eObj, &EffectGameObject::m_moveOffset, [](float* xStep, bool isMixed) {
			// 	const char* format = isMixed ? "Mixed Values" : "%.2f";
			// 	return ErGui::DragFloat(ImVec4(255, 66, 66, 255), "Move X", xStep, 1.f, 10.f, format);
			// }, true)) {

			// }

			if (ErGui::DragFloat(ImVec4(255, 66, 66, 255), "Move X", &xStep, 1.f, 10.f, "%.2f")) {
				eObj->m_moveOffset.x = xStep * modStep;
				auto field = eObj->m_moveOffset.x;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveOffset.x, field, EffectGameObject);
			}
		}
		else {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			syncObjectProperty(objArr, eObj, &EffectGameObject::m_moveModX, [](float* modX, bool isMixed){
				return ErGui::DragFloat(ImVec4(255, 66, 66, 255), "Mod X", modX, 0.1f, 0.5f, "%.3f", 0.1f, 0.f, 0.f, 0, isMixed);
			});

			// if (ErGui::DragFloat(ImVec4(255, 66, 66, 255), "Mod X", &eObj->m_moveModX, 0.1f, 0.5f, "%.3f", 0.1f)) {
			// 	auto field = eObj->m_moveModX;
			// 	APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveModX, field, EffectGameObject);
			// }
		}

		ImGui::BeginDisabled(eObj->m_isSilent);
		ImGui::Text("Lock X");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		if (ImGui::Checkbox("Player##Lock-X", &eObj->m_lockToPlayerX)) {
			auto field = eObj->m_lockToPlayerX;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerX, field, EffectGameObject);
			if (eObj->m_lockToCameraX) {
				eObj->m_lockToCameraX = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraX, false, EffectGameObject);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Camera##Lock-X", &eObj->m_lockToCameraX)) {
			auto field = eObj->m_lockToCameraX;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraX, field, EffectGameObject);
			if (eObj->m_lockToPlayerX) {
				eObj->m_lockToPlayerX = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerX, false, EffectGameObject);
			}
		}
		ImGui::EndDisabled();

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (!eObj->m_lockToCameraY && !eObj->m_lockToPlayerY) {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			if (ErGui::DragFloat(ImVec4(66, 66, 255, 255), "Move Y", &yStep, 1.f, 10.f, "%.2f")) {
				eObj->m_moveOffset.y = yStep * modStep;
				auto field = eObj->m_moveOffset.y;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveOffset.y, field, EffectGameObject);
			}
		}
		else {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			syncObjectProperty(objArr, eObj, &EffectGameObject::m_moveModY, [](float* modY, bool isMixed){
				return ErGui::DragFloat(ImVec4(66, 66, 255, 255), "Mod Y", modY, 0.1f, 0.5f, "%.3f", 0.1f, 0.f, 0.f, 0, isMixed);
			});
			// if (ErGui::DragFloat(ImVec4(66, 66, 255, 255), "Mod Y", &eObj->m_moveModY, 0.1f, 0.5f, "%.3f", 0.1f)) {
			// 	auto field = eObj->m_moveModY;
			// 	APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveModY, field, EffectGameObject);
			// }
		}

		ImGui::BeginDisabled(eObj->m_isSilent);
		ImGui::Text("Lock Y");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		if (ImGui::Checkbox("Player##Lock-Y", &eObj->m_lockToPlayerY)) {
			auto field = eObj->m_lockToPlayerY;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerY, field, EffectGameObject);
			if (eObj->m_lockToCameraY) {
				eObj->m_lockToCameraY = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraY, false, EffectGameObject);
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Camera##Lock-Y", &eObj->m_lockToCameraY)) {
			auto field = eObj->m_lockToCameraY;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraY, field, EffectGameObject);
			if (eObj->m_lockToPlayerY) {
				eObj->m_lockToPlayerY = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerY, false, EffectGameObject);
			}
		}
		ImGui::EndDisabled();

	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Center Group ID", &eObj->m_targetModCenterID)) {
			auto targetModCenter = eObj->m_targetModCenterID;
			if (eObj->m_targetModCenterID < 0)		eObj->m_targetModCenterID = 0;
			if (eObj->m_targetModCenterID > 9999)	eObj->m_targetModCenterID = 9999;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_targetModCenterID, targetModCenter, EffectGameObject);
		}
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Target Pos ID", &eObj->m_centerGroupID)) {
			auto centerGroup = eObj->m_centerGroupID;
			if (eObj->m_centerGroupID < 0)		eObj->m_centerGroupID = 0;
			if (eObj->m_centerGroupID > 9999)	eObj->m_centerGroupID = 9999;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_centerGroupID, centerGroup, EffectGameObject);
		}

		if (ImGui::Checkbox("P1", &eObj->m_targetPlayer1) && eObj->m_targetPlayer2) {
			eObj->m_targetPlayer2 = false;
			for (auto objInArr : CCArrayExt<EffectGameObject*>(objArr)) {
				objInArr->m_targetPlayer1 = eObj->m_targetPlayer1;
				objInArr->m_targetPlayer2 = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("P2", &eObj->m_targetPlayer2) && eObj->m_targetPlayer1) {
			eObj->m_targetPlayer1 = false;
			for (auto objInArr : CCArrayExt<EffectGameObject*>(objArr)) {
				objInArr->m_targetPlayer2 = eObj->m_targetPlayer2;
				objInArr->m_targetPlayer1 = false;
			}
		}


		if (eObj->m_isDirectionFollowSnap360) {
			float moveDistance = eObj->m_directionModeDistance / modStep;
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::InputFloat("Distance", &moveDistance, 1.f, 10.f, "%.2f");
			auto field = moveDistance * modStep;
			eObj->m_directionModeDistance = field;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_directionModeDistance, field, EffectGameObject);
		}
		else {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			if (ImGui::Combo("Target Mode", reinterpret_cast<int*>(&eObj->m_moveTargetMode), moveTargetModeItems, IM_ARRAYSIZE(moveTargetModeItems))) {
				auto field = eObj->m_moveTargetMode;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveTargetMode, field, EffectGameObject);
			}
		}
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Use Target", &eObj->m_useMoveTarget)) {
		auto field = eObj->m_useMoveTarget;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_useMoveTarget, field, EffectGameObject);
		if (eObj->m_isDirectionFollowSnap360) {
			eObj->m_isDirectionFollowSnap360 = false;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDirectionFollowSnap360, false, EffectGameObject);
		}
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Dynamic Mode", &eObj->m_isDynamicMode)) {
		auto field = eObj->m_isDynamicMode;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDynamicMode, field, EffectGameObject);
	}
	if (ImGui::Checkbox("Use Direction", &eObj->m_isDirectionFollowSnap360)) {
		auto field = eObj->m_isDirectionFollowSnap360;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDirectionFollowSnap360, field, EffectGameObject);
		if (eObj->m_useMoveTarget) {
			eObj->m_useMoveTarget = false;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_useMoveTarget, false, EffectGameObject);
		}
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Small Step", &eObj->m_smallStep)) {
		auto field = eObj->m_smallStep;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_smallStep, field, EffectGameObject);
	}

	if (ImGui::Checkbox("Silent", &eObj->m_isSilent)) {
		auto field = eObj->m_isSilent;
		if (field) {
			eObj->m_lockToCameraX = false;
			eObj->m_lockToPlayerX = false;
			eObj->m_lockToCameraY = false;
			eObj->m_lockToPlayerY = false;

			for (auto objInArr : CCArrayExt<EffectGameObject*>(objArr)) {
				objInArr->m_lockToCameraX = false;
				objInArr->m_lockToPlayerX = false;
				objInArr->m_lockToCameraY = false;
				objInArr->m_lockToPlayerY = false;
			}
		}

		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isSilent, field, EffectGameObject);
	}

	if (!eObj->m_isSilent) {
		drawComponentTime(eObj, objArr, "Move Time", true);
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawStopSettings(GameObject* obj, CCArray* objArr) {
	auto tcObj = static_cast<TriggerControlGameObject*>(obj);
	auto lel = LevelEditorLayer::get();
	
	drawComponentGroupID(tcObj, objArr, "Group ID");
	drawTouchSpawnTriggered(tcObj, objArr);
}

void drawPulseSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	if (ImGui::Combo("Target Type", reinterpret_cast<int*>(&eObj->m_pulseTargetType), pulseTargetTypeItems, IM_ARRAYSIZE(pulseTargetTypeItems))) {
		if (eObj->m_pulseTargetType == 0 && eObj->m_targetGroupID > 1101) {
			eObj->m_targetGroupID = 1101;
			lel->updateObjectLabel(obj);
		}
	}
	ImGui::Checkbox("Exclusive", &eObj->m_pulseExclusive);
	if (eObj->m_pulseTargetType == 0) {
		auto targetColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(eObj->m_targetGroupID);
		if (ImGui::ColorButton("##TARGETCOLOR", ImVec4(targetColorAction->m_fromColor.r / 255.f, targetColorAction->m_fromColor.g / 255.f, targetColorAction->m_fromColor.b / 255.f, 1.f))) {
			ImGui::OpenPopup("CSP##TARGET"); // openpopup
		}
		ErGui::colorSelectImGuiPopup(&eObj->m_targetGroupID, "CSP##TARGET", false, eObj);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.f);
		int targetID = eObj->m_targetGroupID;
		if (ImGui::InputInt(("Target Color##INPUT-TARGETCOLOR"), &targetID)) {
			if (targetID > 1101)					targetID = 1101;
			if (targetID < 0)						targetID = 0;

			eObj->m_targetGroupID = targetID;
			lel->updateObjectLabel(obj);
		}
	}
	else {
		if (ImGui::Checkbox("Main Only", &eObj->m_pulseMainOnly) && eObj->m_pulseDetailOnly) {
			eObj->m_pulseDetailOnly = false;
		}
		if (ImGui::Checkbox("Detail Only", &eObj->m_pulseDetailOnly) && eObj->m_pulseMainOnly) {
			eObj->m_pulseMainOnly = false;
		}
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
			if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
			if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
			lel->updateObjectLabel(obj);
		}
	}

	ImGui::Separator();

	ImGui::Combo("Pulse Mode", reinterpret_cast<int*>(&eObj->m_pulseMode), pulseModeItems, IM_ARRAYSIZE(pulseModeItems));
	if (eObj->m_pulseMode == 0) {
		float r, g, b;
		float hsv[4] = { savedHueEO, savedSaturationEO, savedValueEO, eObj->m_opacity };
		ImGuiColorEditFlags flags =
			ImGuiColorEditFlags_InputHSV | ImGuiColorEditFlags_AlphaBar;
		if (geode::Mod::get()->getSavedValue<bool>("triangle-color-wheel", true))
			flags |= ImGuiColorEditFlags_PickerHueWheel;
		if (!geode::Mod::get()->getSavedValue<bool>("rotate-color-wheel", false))
			flags |= ImGuiColorEditFlags_DisableWheelRot;

		ImGui::ColorPicker4(std::string("##PULSE-SETTINGS").c_str(), hsv, flags);

		savedHueEO = hsv[0];
		savedSaturationEO = hsv[1];
		savedValueEO = hsv[2];
		eObj->m_opacity = hsv[3];

		ImGui::ColorConvertHSVtoRGB(savedHueEO, savedSaturationEO, savedValueEO, r, g, b);
		eObj->m_triggerTargetColor = ccColor3B(int(r * 255), int(g * 255), int(b * 255));
	}
	else {
		ImGui::DragFloat("Hue", &eObj->m_hsvValue.h, 1.f, -180.f, 180.f);
		ImGui::DragFloat("Saturation", &eObj->m_hsvValue.s, 0.05f, -1.f, 2.f);
		ImGui::DragFloat("Value", &eObj->m_hsvValue.v, 0.05f, -1.f, 2.f);

		ImGui::Checkbox("Absolute Saturation##AS", &eObj->m_hsvValue.absoluteSaturation);
		ImGui::Checkbox("Absolute Value##AV", &eObj->m_hsvValue.absoluteBrightness);
		ImGui::Checkbox("Legacy HSV", &eObj->m_legacyHSV);

		ErGui::clampHSV(&eObj->m_hsvValue);

		if (ImGui::Button("HSV Reset##HSV-RESET")) {
			eObj->m_hsvValue.absoluteBrightness = false;
			eObj->m_hsvValue.absoluteSaturation = false;
			eObj->m_hsvValue.h = 0.f;
			eObj->m_hsvValue.s = 1.f;
			eObj->m_hsvValue.v = 1.f;
		}

		auto hsvColorAction = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager->getColorAction(eObj->m_copyColorID);
		if (ImGui::ColorButton("##COPYCOLOR", ImVec4(hsvColorAction->m_fromColor.r / 255.f, hsvColorAction->m_fromColor.g / 255.f, hsvColorAction->m_fromColor.b / 255.f, 1.f))) {
			ImGui::OpenPopup("CSP##COPY");
		}
		ErGui::colorSelectImGuiPopup(&eObj->m_copyColorID, "CSP##COPY", false, eObj);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.f);
		int hsvColorID = eObj->m_copyColorID;
		if (ImGui::InputInt(("Copy Color##INPUT-COPYCOLOR"), &hsvColorID)) {
			if (hsvColorID > 1101)					hsvColorID = 1101;
			if (hsvColorID < 0)						hsvColorID = 0;

			eObj->m_copyColorID = hsvColorID;
		}
	}

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Fade In", &eObj->m_fadeInDuration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_fadeInDuration < 0.f) eObj->m_fadeInDuration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Hold", &eObj->m_holdDuration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_holdDuration < 0.f) eObj->m_holdDuration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Fade Out", &eObj->m_fadeOutDuration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_fadeOutDuration < 0.f) eObj->m_fadeOutDuration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawAlphaSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Group ID");

	ImGui::SeparatorText("Core Settings");
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("Opacity", &eObj->m_opacity, 0.1f, 0.5f, "%.2f", 0.05f, 0.f, 1.f, ImGuiSliderFlags_ClampZeroRange)) {
		auto opacity = eObj->m_opacity;
		if (opacity < 0.f) opacity = 0.f;
		if (opacity > 1.f) opacity = 1.f;
		eObj->m_opacity = opacity;

		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_opacity, opacity, EffectGameObject)
	}

	drawComponentTime(eObj, objArr, "Fade Time", false);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawToggleSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Group ID");

	if (ImGui::Checkbox("Activate Group", &eObj->m_activateGroup) && eObj->getChildByType<CCSprite*>(0)) {
		if (!eObj->m_activateGroup) {
			eObj->getChildByType<CCSprite*>(0)->setColor({ 255, 63, 63 });
			
			// Multi Edit
			for (auto objInArr : CCArrayExt<EffectGameObject*>(objArr)) {
				objInArr->m_activateGroup = false;
				objInArr->getChildByType<CCSprite*>(0)->setColor({ 255, 63, 63 });
			}
		}
		else {
			eObj->getChildByType<CCSprite*>(0)->setColor({ 0, 255, 127 });
			
			// Multi Edit
			for (auto objInArr : CCArrayExt<EffectGameObject*>(objArr)) {
				objInArr->m_activateGroup = true;
				eObj->getChildByType<CCSprite*>(0)->setColor({ 0, 255, 127 });
			}
		}
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawSpawnSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<SpawnTriggerGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Group ID");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Delay", &eObj->m_spawnDelay, 0.1f, 0.5f, "%.2f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Range##Delay", &eObj->m_delayRange, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_delayRange < 0.f) eObj->m_delayRange = 0.f;
	}

	ImGui::Checkbox("Preview Disable", &eObj->m_previewDisable);
	ImGui::Checkbox("Spawn Ordered", &eObj->m_spawnOrdered);
	ImGui::Checkbox("Reset Remap", &eObj->m_resetRemap);

	drawTouchSpawnTriggered(eObj, objArr);

	if (ImGui::CollapsingHeader("Remap Settings")) {

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Original ID", &spawnOldGroupID)) {
			if (spawnOldGroupID < 0) spawnOldGroupID = 0;
			if (spawnOldGroupID > 9999) spawnOldGroupID = 9999;
		}

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("New ID", &spawnNewGroupID)) {
			if (spawnNewGroupID < 0) spawnNewGroupID = 0;
			if (spawnNewGroupID > 9999) spawnNewGroupID = 9999;
		}

		if (ImGui::Button("Add") && spawnOldGroupID != spawnNewGroupID && spawnOldGroupID > 0 && spawnNewGroupID > 0) {
			bool foundSame = false;
			for (auto cObj : eObj->m_remapObjects) {
				if (cObj.m_groupID == spawnOldGroupID && cObj.m_chance == spawnNewGroupID) {
					foundSame = true;
					break;
				}
			}

			if (!foundSame) {
				ChanceObject newCObj;
				newCObj.m_oldGroupID = spawnOldGroupID;
				newCObj.m_groupID = spawnOldGroupID;
				newCObj.m_chance = spawnNewGroupID;
				newCObj.m_unk00c = 0;

				eObj->m_remapObjects.push_back(newCObj);
			}
		}

		ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });
		ImGui::SeparatorText("Remaps");
		ImGui::PopStyleColor();

		for (int i = 0; i < eObj->m_remapObjects.size(); i++) {
			auto cObj = eObj->m_remapObjects[i];
			std::string btnStr = fmt::format("{} -> {}", cObj.m_groupID, cObj.m_chance);
			if (ImGui::Button(btnStr.c_str())) {
				eObj->m_remapObjects.erase(eObj->m_remapObjects.begin() + i);
			}
			if ((i + 1) % 4 != 0 && i + 1 != eObj->m_remapObjects.size()) ImGui::SameLine();
		}
	}


}

void drawRotateSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EnhancedTriggerObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Target ID", "Center ID");

	ImGui::Separator();

	if (ImGui::Checkbox("Aim Mode", &eObj->m_useMoveTarget) && eObj->m_isDirectionFollowSnap360) {
		eObj->m_isDirectionFollowSnap360 = false;
	}
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Dynamic Mode", &eObj->m_isDynamicMode);
	if (ImGui::Checkbox("Follow Mode", &eObj->m_isDirectionFollowSnap360) && eObj->m_useMoveTarget) {
		eObj->m_useMoveTarget = false;
	}
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Lock Obj Rotation", &eObj->m_lockObjectRotation);

	ImGui::Separator();

	if (!eObj->m_useMoveTarget && !eObj->m_isDirectionFollowSnap360) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputFloat("Degrees", &eObj->m_rotationDegrees, 1.f, 10.f, "%.2f");

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputInt("x360", &eObj->m_times360, 1, 5);

	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Rot Target ID", &eObj->m_rotationTargetID)) {
			if (eObj->m_rotationTargetID < 0)		eObj->m_rotationTargetID = 0;
			if (eObj->m_rotationTargetID > 9999)	eObj->m_rotationTargetID = 9999;
		}
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputFloat("Rot Offset", &eObj->m_rotationOffset, 1.f, 10.f, "%.2f");


		if (ImGui::Checkbox("P1", &eObj->m_targetPlayer1) && eObj->m_targetPlayer2) {
			eObj->m_targetPlayer2 = false;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("P2", &eObj->m_targetPlayer2) && eObj->m_targetPlayer1) {
			eObj->m_targetPlayer1 = false;
		}

		if (eObj->m_useMoveTarget) {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2);
			if (ImGui::InputInt("##MinXID", &eObj->m_minXID)) {
				if (eObj->m_minXID < 0)		eObj->m_minXID = 0;
				if (eObj->m_minXID > 9999)	eObj->m_minXID = 9999;

			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2);
			if (ImGui::InputInt("MinMax X ID##MaxXID", &eObj->m_maxXID)) {
				if (eObj->m_maxXID < 0)		eObj->m_maxXID = 0;
				if (eObj->m_maxXID > 9999)	eObj->m_maxXID = 9999;
			}

			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2);
			if (ImGui::InputInt("##MinYID", &eObj->m_minYID)) {
				if (eObj->m_minYID < 0)		eObj->m_minYID = 0;
				if (eObj->m_minYID > 9999)	eObj->m_minYID = 9999;
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2);
			if (ImGui::InputInt("MinMax Y ID##MaxYID", &eObj->m_maxYID)) {
				if (eObj->m_maxYID < 0)		eObj->m_maxYID = 0;
				if (eObj->m_maxYID > 9999)	eObj->m_maxYID = 9999;
			}
		}
	}


	if (eObj->m_isDynamicMode && (eObj->m_useMoveTarget || eObj->m_isDirectionFollowSnap360)) {
		drawComponentTime(eObj, objArr, "Move Time", false);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputInt("Easing", reinterpret_cast<int*>(&eObj->m_dynamicModeEasing), 1, 10);
		//ImGui::Text(std::to_string(static_cast<int>(eObj->m_dynamicModeEasing)).c_str());
	}
	else {
		drawComponentTime(eObj, objArr, "Move Time", true);
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawScaleSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<TransformTriggerGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Target ID", "Center ID");

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Scale X", &eObj->m_objectScaleX, 0.1f, 0.5f, "%.2f");
	ImGui::Checkbox("Div by X", &eObj->m_divideX);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Scale Y", &eObj->m_objectScaleY, 0.1f, 0.5f, "%.2f");
	ImGui::Checkbox("Div by Y", &eObj->m_divideY);

	ImGui::Separator();

	ImGui::Checkbox("Only Move", &eObj->m_onlyMove);
	ImGui::Checkbox("Relative Scale", &eObj->m_relativeScale);
	ImGui::Checkbox("Relative Rotation", &eObj->m_relativeRotation);

	drawComponentTime(eObj, objArr, "Scale Time", true);
	drawTouchSpawnTriggered(eObj, objArr);
}

void drawAnimateSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Group ID");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::DragInt("Animation ID", &eObj->m_animationID, 1, 5, "%d", 1, 0, 9, ImGuiSliderFlags_ClampZeroRange)) {
		auto animationID = eObj->m_animationID;
		if (animationID < 0) eObj->m_animationID = 0;
		if (animationID > 9) eObj->m_animationID = 9;
		animationID = eObj->m_opacity;

		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_animationID, animationID, EffectGameObject)
	}

	drawTouchSpawnTriggered(eObj, objArr);

	if (ImGui::CollapsingHeader("-----| Animation IDs hint |-----")) {
		ImGui::TextWrapped("Big Beast");
		ImGui::TextWrapped("0 - Bite | 1 - Attack | 2 - Attack End | 3 - Idle");
		ImGui::TextWrapped("\nBat");
		ImGui::TextWrapped("0 - Idle1 | 1 - Idle2 | 2 - Idle3");
		ImGui::TextWrapped("3 - Attack1 | 4 - Attack2 | 5 - Attack2 End | 9 - Attack2 Loop");
		ImGui::TextWrapped("6 - Sleep | 7 - Sleep Loop | 8 - Sleep End");
		ImGui::TextWrapped("\nSpikeball");
		ImGui::TextWrapped("0 - Idle1 | 1 - Idle2 | 7 - Idle3");
		ImGui::TextWrapped("2 - ToAttack1 | 3 - Attack1 | 4 - Attack2");
		ImGui::TextWrapped("5 - ToAttack3 | 6 - Attack3 | 8 - FromAttack3");
	}
}

void drawTouchSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	drawComponentGroupID(eObj, objArr, "Group ID");

	bool p1 = (int)eObj->m_touchPlayerMode == 1;
	bool p2 = (int)eObj->m_touchPlayerMode == 2;

	if (ImGui::Checkbox("P1 Only", &p1)) {
		p2 = false;
		if (p1) (int&)eObj->m_touchPlayerMode = 1;
		else (int&)eObj->m_touchPlayerMode = 0;
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("P2 Only", &p2)) {
		p1 = false;
		if (p2) (int&)eObj->m_touchPlayerMode = 2;
		else (int&)eObj->m_touchPlayerMode = 0;
	}

	bool toggleOn = (int)eObj->m_touchToggleMode == 1;
	bool toggleOff = (int)eObj->m_touchToggleMode == 2;

	if (ImGui::Checkbox("Toggle On", &toggleOn)) {
		toggleOff = false;
		if (toggleOn) (int&)eObj->m_touchToggleMode = 1;
		else (int&)eObj->m_touchToggleMode = 0;
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Toggle Off", &toggleOff)) {
		toggleOn = false;
		if (toggleOff) (int&)eObj->m_touchToggleMode = 2;
		else (int&)eObj->m_touchToggleMode = 0;
	}

	ImGui::Checkbox("Hold Mode", &eObj->m_touchHoldMode);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Dual Mode", &eObj->m_isDualMode);

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawRandomSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	drawComponentGroupID(eObj, objArr, "Group ID 1", "Group ID 2");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("Chance", &eObj->m_duration, 1.f, 5.f, "%.0f%%", 1.f, 0.f, 100.f)) {
		auto chance = eObj->m_duration;
		if (chance < 0.f) eObj->m_duration = 0.f;
		if (chance > 100.f) eObj->m_duration = 100.f;
		chance = eObj->m_duration;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_duration, chance, EffectGameObject)
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawSpawnParticleSettings(GameObject* obj, CCArray* objArr) {

	auto lel = LevelEditorLayer::get();
	auto eObj = static_cast<SpawnParticleGameObject*>(obj);

	//ImGui::DragFloat("Top", &dummy.x);
	//ImGui::DragFloat("Down", &dummy.y);

	drawComponentGroupID(eObj, objArr, "Particle ID", "Position ID");

	SeparatorPlus("Position");

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat(ImVec4(255, 66, 66, 255), "Offset##Offset X", &eObj->m_offset.x)) {
		float offsetX = eObj->m_offset.x;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_offset.x, offsetX, SpawnParticleGameObject);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat(ImVec4(66, 66, 255, 255), "##Offset Y", &eObj->m_offset.y)) {
		float offsetY = eObj->m_offset.y;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_offset.y, offsetY, SpawnParticleGameObject);
	}

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat(ImVec4(175, 50, 50, 255), "OffVar##OffVar X", &eObj->m_offsetVariance.x)) {
		float offVarX = eObj->m_offsetVariance.x;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_offsetVariance.x, offVarX, SpawnParticleGameObject);
	}
	
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat(ImVec4(50, 74, 178, 255), "##OffVar Y", &eObj->m_offsetVariance.y)) {
		float offVarY = eObj->m_offsetVariance.y;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_offsetVariance.y, offVarY, SpawnParticleGameObject);
	}

	SeparatorPlus("Rotation");

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("RotateDir", &eObj->m_rotation)) {
		float rotateDir = eObj->m_rotation;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_rotation, rotateDir, SpawnParticleGameObject);
	}


	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("Rotate Var", &eObj->m_rotationVariance)) {
		float rotationVar = eObj->m_rotationVariance;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_rotationVariance, rotationVar, SpawnParticleGameObject);
	}

	if (ImGui::Checkbox("Match Rot", &eObj->m_matchRotation)) {
		float match = eObj->m_matchRotation;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_matchRotation, match, SpawnParticleGameObject);
	}

	SeparatorPlus("Scale");

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("Scale", &eObj->m_scale, 1.0f, 5.0f, "%.2f", 0.1f)) {
		float scale = eObj->m_scale;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_scale, scale, SpawnParticleGameObject);
	}

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("ScaleVar", &eObj->m_scaleVariance, 1.0f, 5.0f, "%.2f", 0.1f)) {
		float scaleVar = eObj->m_scaleVariance;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_scaleVariance, scaleVar, SpawnParticleGameObject);
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawReset(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	drawComponentGroupID(eObj, objArr, "Group ID");
	drawTouchSpawnTriggered(eObj, objArr);
}

void drawReverseSettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<UISettingsGameObject*>(obj);
	drawTouchSpawnTriggered(eObj, objArr);
}

void drawTimeWarp(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("Time Modifier", &eObj->m_timeWarpTimeMod, 0.1f, 0.5f, "%.2f", 0.05f, .1f, 2.f)) {
		auto time = eObj->m_timeWarpTimeMod;
		if (time < 0.1f) eObj->m_timeWarpTimeMod = 0.1f;
		if (time > 2.f) eObj->m_timeWarpTimeMod = 2.f;
		time = eObj->m_timeWarpTimeMod;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_timeWarpTimeMod, time, EffectGameObject);

		auto dgl = GameManager::sharedState()->m_levelEditorLayer->m_drawGridLayer;
		dgl->loadTimeMarkers(dgl->m_timeMarkerString);
	}

	drawTouchSpawnTriggered(eObj, objArr);
}

void drawUISettings(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<UISettingsGameObject*>(obj);
	drawComponentGroupID(eObj, objArr, "Group ID", "UI Center ID");

	ImGui::Checkbox("XRef Relative", &eObj->m_xRelative);
	ImGui::Checkbox("YRef Relative", &eObj->m_yRelative);

	const char* refItemsX[] = { "Auto", "Center", "Left", "Right" };
	const char* refItemsY[] = { "Auto", "Center", "Bottom", "Top" };
	int xItem = eObj->m_xRef - 1;
	int yItem = eObj->m_yRef - 5;

	if (eObj->m_xRef == 0) xItem = 0;
	if (eObj->m_yRef == 0) yItem = 0;

	if (ImGui::Combo("XRef", &xItem, refItemsX, IM_ARRAYSIZE(refItemsX))) {
		eObj->m_xRef = xItem + 1;
	}

	if (ImGui::Combo("YRef", &yItem, refItemsY, IM_ARRAYSIZE(refItemsY))) {
		eObj->m_yRef = yItem + 5;
	}

	//ImGui::Text(std::to_string(eObj->m_xRef).c_str());
	//ImGui::Text(std::to_string(eObj->m_yRef).c_str());
}

void drawVisibilityLink(GameObject* obj, CCArray* objArr) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	drawComponentGroupID(eObj, objArr, "Group ID");
}

void ErGui::mapStandardTriggers() {
	// --- Standart
	triggersMap[31] = drawStartPosSettings;
	triggersMap[899] = drawColorSettings;
	triggersMap[901] = drawMoveSettings;
	triggersMap[1616] = drawStopSettings;
	triggersMap[1006] = drawPulseSettings;
	triggersMap[1007] = drawAlphaSettings;
	triggersMap[1049] = drawToggleSettings;		//Toggle
	triggersMap[1812] = drawToggleSettings;		//OnDeath
	triggersMap[1268] = drawSpawnSettings;
	triggersMap[1346] = drawRotateSettings;
	triggersMap[2067] = drawScaleSettings;
	triggersMap[1585] = drawAnimateSettings;
	triggersMap[1595] = drawTouchSettings;
	triggersMap[1912] = drawRandomSettings;
	// Advanced Random
	// Sequence
	triggersMap[3608] = drawSpawnParticleSettings;
	triggersMap[3618] = drawReset;
	triggersMap[1917] = drawReverseSettings;
	triggersMap[1935] = drawTimeWarp;
	triggersMap[3613] = drawUISettings;
	triggersMap[3662] = drawVisibilityLink;
	// Gradient
}

void ErGui::mapOldColorTriggers() {
	// Old color triggers
	triggersMap[29] = drawColorSettings;
	triggersMap[30] = drawColorSettings;
	triggersMap[105] = drawColorSettings;
	triggersMap[744] = drawColorSettings;
	triggersMap[900] = drawColorSettings;
	triggersMap[915] = drawColorSettings;
}
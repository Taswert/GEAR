#pragma once
#include "EditObjectModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"
#include "EditColorModule.hpp"

#include "misc/cpp/imgui_stdlib.h"
#include "CustomImGuiWidgets.hpp"

const std::unordered_set<int> triggerSet = { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 55, 56, 57, 58, 59, 105, 744, 900, 915, 899, 901, 914, 1616, 1006, 1007, 1049, 1268, 1346, 2067, 1347, 1520, 1585, 1912, 3033, 1814, 1915, 2063, 3016, 3617, 3660, 3661, 3032, 3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024, 3029, 3030, 3031, 1595, 1611, 1811, 1817, 3608, 3614, 3615, 3617, 3619, 3620, 3641, 1912, 2068, 3607, 3608, 3618, 1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016, 1917, 2900, 1934, 3605, 3602, 3603, 3604, 1935, 2999, 3606, 3612, 1615, 3613, 3662, 1815, 3609, 3640, 1816, 3643, 1812, 33, 32, 1613, 1612, 1818, 1819, 3600, 1932, 2899, 3642, 2903, 2066, 3022, 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924, 22, 24, 23, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915, 3017, 3018, 3019, 3020, 3021, 3023, 29, 30, 105, 744, 915, 1931, 3655, 2069, 3645 };
using ErGuiSettingsDrawer = void (*)(GameObject*, CCArray*);
std::unordered_map<int, ErGuiSettingsDrawer> triggersMap;

const char* easingItems[] = { "None",
"Ease In Out",			"Ease In",			"Ease Out",
"Elastic In Out",		"Elastic In",		"Elastic Out",
"Bounce In Out",		"Bounce In",		"Bounce Out",
"Exponential In Out",	"Exponential In",	"Exponential Out",
"Sine In Out",			"Sine In",			"Sine Out",
"Back In Out",			"Back In",			"Back Out"
};

const char* moveTargetModeItems[] = {
	"Both", "X Only", "Y Only"
};

const char* pulseModeItems[] = {
	"Color Mode", "HSV Mode"
};

const char* pulseTargetTypeItems[] = {
	"Channel", "Group"
};

const char* gameModesItems[] = {
	"Cube", "Ship", "Ball", "UFO", "Wave", "Robot", "Spider", "Swing"
};

const char* speedItems[] = {
	"Slow", "Normal", "Fast", "Faster", "Very Fast"
};

const int speedValues[] = {
	1, 0, 2, 3, 4,
};

const char* enterType[] = {
	"None", "Enter Only", "Exit Only"
};

int spawnOldGroupID = 0;
int spawnNewGroupID = 0;

float savedHueEO, savedSaturationEO, savedValueEO;


#define APPLY_FIELDS_TO_OTHER_TRIGGERS(m_field, TriggerClass)	\
auto myField = eObj->m_field;									\
for (auto objInArr : CCArrayExt<TriggerClass*>(objArr)) {		\
	objInArr->m_field = myField;								\
}

// LevelEditorLayer should be defined as lel at this point pls
#define APPLY_FIELDS_TO_OTHER_TRIGGERS_AND_UPDATE(m_field, TriggerClass)	\
auto myField = eObj->m_field;												\
for (auto objInArr : CCArrayExt<TriggerClass*>(objArr)) {					\
	objInArr->m_field = myField;											\
	lel->updateObjectLabel(objInArr);										\
}



void drawTouchSpawnTriggered(EffectGameObject* eObj) {
	ImGui::Checkbox("Touch Trigger", &eObj->m_isTouchTriggered);
	ImGui::Checkbox("Spawn Trigger", &eObj->m_isSpawnTriggered);
	if (eObj->m_isSpawnTriggered || eObj->m_isTouchTriggered)
		ImGui::Checkbox("Multi Trigger", &eObj->m_isMultiTriggered);
}

void drawEasingSettings(EffectGameObject* eObj, float itemsWidth = 0.f) {
	if (itemsWidth != 0) ImGui::SetNextItemWidth(itemsWidth);
	ImGui::Combo("Easing Type", reinterpret_cast<int*>(&eObj->m_easingType), easingItems, IM_ARRAYSIZE(easingItems));
	if (static_cast<int>(eObj->m_easingType) > 0 && static_cast<int>(eObj->m_easingType) < 7) {
		if (itemsWidth != 0) ImGui::SetNextItemWidth(itemsWidth);
		if (ImGui::InputFloat("Easing Rate", &eObj->m_easingRate, 0.1f, 0.5f, "%.2f")) {
			if (eObj->m_easingRate < 0.1f) eObj->m_easingRate = 0.1f;
			if (eObj->m_easingRate > 20.f) eObj->m_easingRate = 20.f;
		}
	}
}

void ErGui::saveHueValues(ccColor3B* color) {
	float r = color->r / 255.f;
	float g = color->g / 255.f;
	float b = color->b / 255.f;
	ImGui::ColorConvertRGBtoHSV(r, g, b, savedHueEO, savedSaturationEO, savedValueEO);
}

void drawStartPosSettings(GameObject* obj) {
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

std::set<int> oldColorTriggers = {
	29, 30, 105, 744, 900, 915
};

inline bool isOldColorTrigger(GameObject* obj) {
	if (oldColorTriggers.contains(obj->m_objectID)) return true;
	else return false;
}

void drawColorSettings(GameObject* obj) {
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

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Fade Time", &eObj->m_duration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_duration < 0.f) eObj->m_duration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
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

	drawTouchSpawnTriggered(eObj);
}

void drawMoveSettings(GameObject* obj, CCArray* objArr) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EnhancedTriggerObject*>(obj);
	

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		APPLY_FIELDS_TO_OTHER_TRIGGERS_AND_UPDATE(m_targetGroupID, EnhancedTriggerObject);
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Use Target", &eObj->m_useMoveTarget) && eObj->m_isDirectionFollowSnap360) {
		eObj->m_isDirectionFollowSnap360 = false;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDirectionFollowSnap360, EnhancedTriggerObject);
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Dynamic Mode", &eObj->m_isDynamicMode)) {
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDynamicMode, EnhancedTriggerObject);
	}
	if (ImGui::Checkbox("Use Direction", &eObj->m_isDirectionFollowSnap360) && eObj->m_useMoveTarget) {
		eObj->m_useMoveTarget = false;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isDirectionFollowSnap360, EnhancedTriggerObject);
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_useMoveTarget, EnhancedTriggerObject);
	}
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox("Small Step", &eObj->m_smallStep)) {
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_smallStep, EnhancedTriggerObject);
	}

	if (ImGui::Checkbox("Silent", &eObj->m_isSilent)) {
		if (eObj->m_isSilent) {
			eObj->m_lockToCameraX = false;
			eObj->m_lockToPlayerX = false;
			eObj->m_lockToCameraY = false;
			eObj->m_lockToPlayerY = false;

			for (auto objInArr : CCArrayExt<EnhancedTriggerObject*>(objArr)) {
				objInArr->m_lockToCameraX = false;
				objInArr->m_lockToPlayerX = false;
				objInArr->m_lockToCameraY = false;
				objInArr->m_lockToPlayerY = false;
			}
		}

		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isSilent, EnhancedTriggerObject);
	}

	ImGui::Separator();

	float modStep = 1.f;
	if (!eObj->m_smallStep) modStep = 3.f;

	if (!eObj->m_useMoveTarget && !eObj->m_isDirectionFollowSnap360) {
		float xStep = eObj->m_moveOffset.x / modStep;
		float yStep = eObj->m_moveOffset.y / modStep;

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (!eObj->m_lockToCameraX && !eObj->m_lockToPlayerX) {
			ImGui::InputFloat("Move X", &xStep, 1.f, 10.f, "%.2f");
			eObj->m_moveOffset.x = xStep * modStep;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveOffset.x, EnhancedTriggerObject);
		}
		else {
			if (ImGui::InputFloat("Mod X", &eObj->m_moveModX, 0.1f, 0.5f, "%.3f")) {
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveModX, EnhancedTriggerObject);
			}
		}

		if (!eObj->m_isSilent) {
			if (ImGui::Checkbox("Player##Lock-X", &eObj->m_lockToPlayerX) && eObj->m_lockToCameraX) {
				eObj->m_lockToCameraX = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraX, EnhancedTriggerObject);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Camera##Lock-X", &eObj->m_lockToCameraX) && eObj->m_lockToPlayerX) {
				eObj->m_lockToPlayerX = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerX, EnhancedTriggerObject);
			}
		}

		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (!eObj->m_lockToCameraY && !eObj->m_lockToPlayerY) {
			ImGui::InputFloat("Move Y", &yStep, 1.f, 10.f, "%.2f");
			eObj->m_moveOffset.y = yStep * modStep;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveOffset.y, EnhancedTriggerObject);
		}
		else {
			if (ImGui::InputFloat("Mod Y", &eObj->m_moveModY, 0.1f, 0.5f, "%.3f")) {
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_moveModY, EnhancedTriggerObject);
			}
			
		}

		if (!eObj->m_isSilent) {
			if (ImGui::Checkbox("Player##Lock-Y", &eObj->m_lockToPlayerY) && eObj->m_lockToCameraY) {
				eObj->m_lockToCameraY = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToCameraY, EnhancedTriggerObject);
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Camera##Lock-Y", &eObj->m_lockToCameraY) && eObj->m_lockToPlayerY) {
				eObj->m_lockToPlayerY = false;
				APPLY_FIELDS_TO_OTHER_TRIGGERS(m_lockToPlayerY, EnhancedTriggerObject);
			}
		}

	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Center Group ID", &eObj->m_targetModCenterID)) {
			if (eObj->m_targetModCenterID < 0)		eObj->m_targetModCenterID = 0;
			if (eObj->m_targetModCenterID > 9999)	eObj->m_targetModCenterID = 9999;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_targetModCenterID, EnhancedTriggerObject);
		}
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputInt("Target Pos ID", &eObj->m_centerGroupID)) {
			if (eObj->m_centerGroupID < 0)		eObj->m_centerGroupID = 0;
			if (eObj->m_centerGroupID > 9999)	eObj->m_centerGroupID = 9999;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_centerGroupID, EnhancedTriggerObject);
		}

		if (ImGui::Checkbox("P1", &eObj->m_targetPlayer1) && eObj->m_targetPlayer2) {
			eObj->m_targetPlayer2 = false;
			for (auto objInArr : CCArrayExt<EnhancedTriggerObject*>(objArr)) {
				objInArr->m_targetPlayer1 = eObj->m_targetPlayer1;
				objInArr->m_targetPlayer2 = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("P2", &eObj->m_targetPlayer2) && eObj->m_targetPlayer1) {
			eObj->m_targetPlayer1 = false;
			for (auto objInArr : CCArrayExt<EnhancedTriggerObject*>(objArr)) {
				objInArr->m_targetPlayer2 = eObj->m_targetPlayer2;
				objInArr->m_targetPlayer1 = false;
			}
		}


		if (eObj->m_isDirectionFollowSnap360) {
			float moveDistance = eObj->m_directionModeDistance / modStep;
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::InputFloat("Distance", &moveDistance, 1.f, 10.f, "%.2f");
			eObj->m_directionModeDistance = moveDistance * modStep;
		}
		else {
			ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
			ImGui::Combo("Target Mode", reinterpret_cast<int*>(&eObj->m_moveTargetMode), moveTargetModeItems, IM_ARRAYSIZE(moveTargetModeItems));
		}
	}

	ImGui::Separator();

	if (!eObj->m_isSilent) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ImGui::InputFloat("Move Time", &eObj->m_duration, 0.1f, 0.5f, "%.2f")) {
			if (eObj->m_duration < -1.f) eObj->m_duration = -1.f;
			auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
			eObj->m_endPosition = *somePoint;
		}

		drawEasingSettings(eObj, ErGui::INPUT_ITEM_WIDTH);
	}

	drawTouchSpawnTriggered(eObj);
}

void drawStopSettings(GameObject* obj) {
	ImGui::Text("Stop");
	auto eObj = static_cast<EffectGameObject*>(obj);
	auto lel = LevelEditorLayer::get();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	auto tcObj = static_cast<TriggerControlGameObject*>(obj);
	//tcObj->targetID


	drawTouchSpawnTriggered(tcObj);
}

void drawPulseSettings(GameObject* obj) {
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

	drawTouchSpawnTriggered(eObj);
}

void drawAlphaSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);


	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
	if (ImGui::InputFloat("Fade Time", &eObj->m_duration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_duration < 0.f) eObj->m_duration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;

		//lel->resetEffectTriggerOptim(obj, lel->m_editorUI->m_selectedObjects); fuck you RobTop. Fuck you and your damn inline funcs. I fucking hate you, dickhead.
	}
	if (ImGui::InputFloat("Opacity", &eObj->m_opacity, 0.f, 0.f, "%.2f")) {
		if (eObj->m_opacity < 0.f) eObj->m_opacity = 0.f;
		if (eObj->m_opacity > 1.f) eObj->m_opacity = 1.f;
	}

	drawTouchSpawnTriggered(eObj);
}

void drawToggleSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	if (ImGui::Checkbox("Activate Group", &eObj->m_activateGroup) && eObj->getChildByType<CCSprite*>(0)) {
		if (!eObj->m_activateGroup) {
			eObj->getChildByType<CCSprite*>(0)->setColor({ 255, 63, 63 });
		}
		else {
			eObj->getChildByType<CCSprite*>(0)->setColor({ 0, 255, 127 });
		}
	}

	drawTouchSpawnTriggered(eObj);
}

void drawSpawnSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<SpawnTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Delay", &eObj->m_spawnDelay, 0.1f, 0.5f, "%.2f");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Range##Delay", &eObj->m_delayRange, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_delayRange < 0.f) eObj->m_delayRange = 0.f;
	}

	ImGui::Checkbox("Preview Disable", &eObj->m_previewDisable);
	ImGui::Checkbox("Spawn Ordered", &eObj->m_spawnOrdered);
	ImGui::Checkbox("Reset Remap", &eObj->m_resetRemap);

	drawTouchSpawnTriggered(eObj);

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
				eObj->m_remapObjects.erase(eObj->m_remapObjects.begin()+i);
			}
			if ((i + 1) % 4 != 0 && i + 1 != eObj->m_remapObjects.size()) ImGui::SameLine();
		}
	}

	
}

void drawRotateSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EnhancedTriggerObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Center ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0)		eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999)	eObj->m_centerGroupID = 9999;
	}

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

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Move Time", &eObj->m_duration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_duration < -1.f) eObj->m_duration = -1.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}

	if (eObj->m_isDynamicMode && (eObj->m_useMoveTarget || eObj->m_isDirectionFollowSnap360)) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::InputInt("Easing", reinterpret_cast<int*>(&eObj->m_dynamicModeEasing), 1, 10);
		//ImGui::Text(std::to_string(static_cast<int>(eObj->m_dynamicModeEasing)).c_str());
	}
	else {
		drawEasingSettings(eObj, ErGui::INPUT_ITEM_WIDTH);
	}

	drawTouchSpawnTriggered(eObj);
}

void drawScaleSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<TransformTriggerGameObject*>(obj);


	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Center ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0)		eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999)	eObj->m_centerGroupID = 9999;
	}

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Scale X", &eObj->m_objectScaleX, 0.1f, 0.5f, "%.2f");
	ImGui::Checkbox("Div by X", &eObj->m_divideX);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputFloat("Scale Y", &eObj->m_objectScaleY, 0.1f, 0.5f, "%.2f");
	ImGui::Checkbox("Div by Y", &eObj->m_divideY);

	ImGui::Separator();

	ImGui::Checkbox("Only Move",			&eObj->m_onlyMove);
	ImGui::Checkbox("Relative Scale",		&eObj->m_relativeScale);
	ImGui::Checkbox("Relative Rotation",	&eObj->m_relativeRotation);

	ImGui::Separator();

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputFloat("Scale Time", &eObj->m_duration, 0.1f, 0.5f, "%.2f")) {
		if (eObj->m_duration < 0.f) eObj->m_duration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;
	}
	drawEasingSettings(eObj, ErGui::INPUT_ITEM_WIDTH);

	drawTouchSpawnTriggered(eObj);
}

void drawAnimateSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	if (ImGui::InputInt("Animation ID", &eObj->m_animationID)) {
		if (eObj->m_animationID < 0) eObj->m_animationID = 0;
		if (eObj->m_animationID > 9) eObj->m_animationID = 9;
	}

	drawTouchSpawnTriggered(eObj);

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

void drawTouchSettings(GameObject* obj) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}

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

	drawTouchSpawnTriggered(eObj);
}

void drawVisibilityLink(GameObject* obj) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}
}

void drawReset(GameObject* obj) {
	auto eObj = static_cast<EffectGameObject*>(obj);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}
	drawTouchSpawnTriggered(eObj);
}

void drawUISettings(GameObject* obj) {
	auto eObj = static_cast<UISettingsGameObject*>(obj);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}

	if (ImGui::InputInt("UI Center ID", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
	}


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

void drawTimeWarp(GameObject* obj) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	if (ImGui::InputFloat("Time Modifier", &eObj->m_timeWarpTimeMod, 0.f, 0.f, "%.2f")) {
		if (eObj->m_timeWarpTimeMod < 0.1f) eObj->m_timeWarpTimeMod = 0.1f;
		if (eObj->m_timeWarpTimeMod > 2.f) eObj->m_timeWarpTimeMod = 2.f;
		auto dgl = GameManager::sharedState()->m_levelEditorLayer->m_drawGridLayer;
		dgl->loadTimeMarkers(dgl->m_timeMarkerString);
	}
	drawTouchSpawnTriggered(eObj);
}

void drawReverseSettings(GameObject* obj) {
	ImGui::Text("Reverse Settings");

	auto eObj = static_cast<UISettingsGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}


void drawTextObjectSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
    auto eObj = static_cast<TextGameObject*>(obj);

    if (ImGui::InputTextMultiline("Text", &eObj->m_text, ImVec2(ErGui::INPUT_ITEM_WIDTH, ImGui::GetFrameHeight() * 2.5f))) {
		eObj->updateTextObject(eObj->m_text, false);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragInt("Kerning", &eObj->m_kerning, 1.f, -10, 20, "%d", 0)) {
		eObj->updateTextKerning(eObj->m_kerning);
		eObj->updateTextObject(eObj->m_text, false);
	}

	if (ImGui::Button("Split")) {
		lel->breakApartTextObject(eObj);
	}
}

void drawEnterEffectSettings(GameObject* obj) {
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

	drawTouchSpawnTriggered(eObj);
}

void drawRandomSettings(GameObject* obj) {
	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID 1", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Group ID 2", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
		lel->updateObjectLabel(obj);
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::DragFloat("Chance", &eObj->m_duration, 1.f, 0.f, 100.f, "%.0f%%")) {
		if (eObj->m_duration < 0.f) eObj->m_duration = 0.f;
		if (eObj->m_duration > 100.f) eObj->m_duration = 100.f;
	}

	drawTouchSpawnTriggered(eObj);
}

void drawSpawnParticleSettings(GameObject* obj) {
	auto eObj = static_cast<SpawnParticleGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Particle Group", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Position Group", &eObj->m_centerGroupID)) {
		if (eObj->m_centerGroupID < 0) eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
	}

	ImGui::Text("Offset X / Y");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ImGui::DragFloat("##OFFSETX", &eObj->m_offset.x, 1.f, -100.f, 1.f, "%.0f");

	ImGui::Checkbox("Match Rotation", &eObj->m_matchRotation);

	drawTouchSpawnTriggered(eObj);
}

void drawTimeControlSettings(GameObject* obj) {
	auto eObj = static_cast<TimerTriggerGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Item ID", &eObj->m_itemID);

	ImGui::Checkbox("Start / Stop", &eObj->m_stopTimeEnabled);

	drawTouchSpawnTriggered(eObj);
}

void checkpointSettings(GameObject* obj) {
	auto cObj = static_cast<CheckpointGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Spawn ID", &cObj->m_targetGroupID)) {
		if (cObj->m_targetGroupID < 0) cObj->m_targetGroupID = 0;
		if (cObj->m_targetGroupID > 9999) cObj->m_targetGroupID = 9999;
	}

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::InputInt("Target Pos", &cObj->m_centerGroupID)) {
		if (cObj->m_centerGroupID < 0) cObj->m_centerGroupID = 0;
		if (cObj->m_centerGroupID > 9999) cObj->m_centerGroupID = 9999;
	}

	ImGui::Checkbox("Player Pos", &cObj->m_targetPlayer1);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Respawn ID", &cObj->m_respawnID);

	drawTouchSpawnTriggered(cObj);
}

void forceBlockSettings(GameObject* obj) {
	auto fObj = static_cast<ForceBlockGameObject*>(obj);

	if (!fObj->m_forceRange) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("Force", &fObj->m_force, .05f, 0.f, 1.f);
	}
	else {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("##MINFORCE", &fObj->m_minForce, .05f, 0.f, 1.f);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH / 2.f);
		ImGui::DragFloat("Min / Max Force", &fObj->m_maxForce, .05f, 0.f, 1.f);
	}

	ImGui::Checkbox("Relative", &fObj->m_relativeForce);
	ImGui::SameLine(150.f);
	ImGui::Checkbox("Range", &fObj->m_forceRange);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::InputInt("Force ID", &fObj->m_forceID);
}

void drawFollowPlayerY(GameObject* obj) {
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

void drawGravitySettings(GameObject* obj) {
	auto eObj = static_cast<EffectGameObject*>(obj);

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Gravity", &eObj->m_gravityValue, .05f, .1f, 1.f);

	if (ImGui::Checkbox("P1", &eObj->m_targetPlayer1)) {
		eObj->m_targetPlayer2 = false;
		eObj->m_followCPP = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("P2", &eObj->m_targetPlayer2)) {
		eObj->m_targetPlayer1 = false;
		eObj->m_followCPP = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("PT", &eObj->m_followCPP)) {
		eObj->m_targetPlayer1 = false;
		eObj->m_targetPlayer2 = false;
	}
	
	drawTouchSpawnTriggered(eObj);
}

void gameOptionsCheckbox(GameOptionsSetting &setting, const char* label) {
	bool off = static_cast<int>(setting == GameOptionsSetting::Off);
	bool on = static_cast<int>(setting == GameOptionsSetting::On);

	ImGui::Text(label);
	ImGui::SameLine(150.f);
	if (ImGui::Checkbox(std::string("On##" + (std::string)label).c_str(), &on)) {
		off = false;
		if (on) setting = GameOptionsSetting::On;
		else setting = GameOptionsSetting::Disabled;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox(std::string("Off##" + (std::string)label).c_str(), &off)) {
		on = false;
		if (off) setting = GameOptionsSetting::Off;
		else setting = GameOptionsSetting::Disabled;
	}
}

void drawGameOptionsSettings(GameObject* obj) {
	auto gObj = static_cast<GameOptionsTrigger*>(obj);

	gameOptionsCheckbox(gObj->m_streakAdditive, "Streak Additive");
	gameOptionsCheckbox(gObj->m_hideGround, "Hide Ground");
	gameOptionsCheckbox(gObj->m_hideMG, "Hide Middle Ground");
	gameOptionsCheckbox(gObj->m_hideP1, "Hide Player 1");
	gameOptionsCheckbox(gObj->m_hideP2, "Hide Player 2");
	gameOptionsCheckbox(gObj->m_disableP1Controls, "Disable P1 Controls");
	gameOptionsCheckbox(gObj->m_disableP2Controls, "Disable P2 Controls");
	gameOptionsCheckbox(gObj->m_unlinkDualGravity, "Unlink Dual Gravity");
	gameOptionsCheckbox(gObj->m_hideAttempts, "Hide Attempts");
	gameOptionsCheckbox(gObj->m_audioOnDeath, "Audio on Death");
	gameOptionsCheckbox(gObj->m_noDeathSFX, "No Death SFX");
	gameOptionsCheckbox(gObj->m_editRespawnTime, "Edit Respawn Time");
	gameOptionsCheckbox(gObj->m_boostSlide, "Boost Slide");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	ImGui::DragFloat("Respawn Time", &gObj->m_respawnTime, .05f, .1f, 10.f);

	drawTouchSpawnTriggered(gObj);
}

void renderObjectSettings(GameObject* obj) {
	int objId = obj->m_objectID;
	if (triggerSet.contains(objId)) {
		if (triggersMap[objId]) {
			ErGuiSettingsDrawer drawTriggerSettings = triggersMap[objId];
			(*drawTriggerSettings)(obj, nullptr);
		}
		else {
			ImGui::Text("Unknown trigger object...");
		}
	}
	else {
		ImGui::Text("Not a trigger...");
	}

	auto editorUI = EditorUI::get();
	if (ImGui::CollapsingHeader("Vanilla Settings")) {
		if (ImGui::Button("Edit Object")) {
			editorUI->editObject(nullptr);
		}
		if (ImGui::Button("Edit Special")) {
			editorUI->editObjectSpecial(0);
		}
	}
}

void renderMultiObjectSettings(CCArray* objArr) {
	auto editorUI = EditorUI::get();
	static int localSelectVersion = ErGui::g_selectVersion;
	static int objId = 0;

	if (localSelectVersion != ErGui::g_selectVersion) {
		localSelectVersion = ErGui::g_selectVersion;
		objId = static_cast<GameObject*>(objArr->objectAtIndex(0))->m_objectID;
		if (!triggerSet.contains(objId)) {
			ImGui::Text("Not a trigger...");
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				if (obj->m_objectID != objId) {
					objId = 0;
					break;
				}
			}
		}
	}
	else if (objId != 0) {
		ErGuiSettingsDrawer drawTriggerSettings = triggersMap[objId];
		(*drawTriggerSettings)(static_cast<GameObject*>(objArr->objectAtIndex(0)), objArr);
	}
	else {
		ImGui::Text("Multi Edit not Compatible...");
		ImGui::Text("%d, %d", localSelectVersion, objId);
	}


	if (ImGui::CollapsingHeader("Vanilla Settings")) {
		if (ImGui::Button("Edit Object")) {
			editorUI->editObject(nullptr);
		}
		if (ImGui::Button("Edit Special")) {
			editorUI->editObjectSpecial(0);
		}
	}
}

void ErGui::renderEditObjectModule() {
	if (!ImGui::Begin("Edit")) {
		ImGui::End();
		return;
	}
	auto editorUI = EditorUI::get();
	if (auto obj = editorUI->m_selectedObject) {
		renderObjectSettings(obj);
	}
	else if (editorUI->m_selectedObjects->count() > 0) {
		auto objArr = editorUI->m_selectedObjects;
		renderMultiObjectSettings(objArr);
	}
	else {
		ImGui::Text("Object is not selected...");
	}
	ImGui::End();
}



void ErGui::setupTriggersSettings() {
	triggersMap[31] = drawStartPosSettings;
	triggersMap[899] = drawColorSettings;
	triggersMap[901] = drawMoveSettings;
	triggersMap[914] = drawTextObjectSettings;
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
	triggersMap[3662] = drawVisibilityLink;
	triggersMap[3618] = drawReset;
	triggersMap[3613] = drawUISettings;
	triggersMap[1935] = drawTimeWarp;
	triggersMap[1917] = drawReverseSettings;
	triggersMap[1912] = drawRandomSettings;
	//triggersMap[3608] = drawSpawnParticleSettings;
	//triggersMap[3617] = drawTimeControlSettings;
	triggersMap[2063] = checkpointSettings;
	triggersMap[2066] = drawGravitySettings;
	triggersMap[2899] = drawGameOptionsSettings;
	
	// Force Blocks
	triggersMap[2069] = forceBlockSettings;
	triggersMap[3645] = forceBlockSettings;


	// Old color triggers
	triggersMap[29] = drawColorSettings;
	triggersMap[30] = drawColorSettings;
	triggersMap[105] = drawColorSettings;
	triggersMap[744] = drawColorSettings;
	triggersMap[900] = drawColorSettings;
	triggersMap[915] = drawColorSettings;

	// Enter Effect Triggers
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

	// Follow
	triggersMap[1814] = drawFollowPlayerY; // Player Y
}


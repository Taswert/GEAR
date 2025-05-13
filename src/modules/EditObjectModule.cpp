#pragma once
#include "EditObjectModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"

std::set<int> triggerSet = { 31, 899, 901, 1616, 1006, 1007, 1049, 1268, 1346, 2067, 1347, 1520, 1585, 3033, 1814, 3016, 3660, 3661, 3032, 3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024, 3029, 3030, 3031, 1595, 1611, 1811, 1817, 3614, 3615, 3617, 3619, 3620, 3641, 1912, 2068, 3607, 3608, 3618, 1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016, 1917, 2900, 1934, 3605, 3602, 3603, 3604, 1935, 2999, 3606, 3612, 1615, 3613, 3662, 1815, 3609, 3640, 1816, 3643, 1812, 33, 32, 1613, 1612, 1818, 1819, 3600, 1932, 2899, 3642, 2903, 2066, 3022, 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924, 22, 24, 23, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915, 3017, 3018, 3019, 3020, 3021, 3023, 29, 30, 105, 744, 915, 1931, 3655, };
using ErGuiSettingsDrawer = void (*)(GameObject*);
std::unordered_map<int, ErGuiSettingsDrawer> triggersMap;

const char* easingItems[] = { "None", 
"Ease In Out",			"Ease In",			"Ease Out",
"Elastic In Out",		"Elastic In",		"Elastic Out", 
"Bounce In Out",		"Bounce In",		"Bounce Out", 
"Exponential In Out",	"Exponential In",	"Exponential Out", 
"Sine In Out",			"Sine In",			"Sine Out", 
"Back In Out",			"Back In",			"Back Out"
};

void drawTouchSpawnTriggered(EffectGameObject* eObj) {
	if (eObj->m_isSpawnTriggered) eObj->m_isTouchTriggered = false;
	ImGui::Checkbox("Touch Trigger", &eObj->m_isTouchTriggered);
	if (eObj->m_isTouchTriggered) eObj->m_isSpawnTriggered = false;
	ImGui::Checkbox("Spawn Trigger", &eObj->m_isSpawnTriggered);
	if (eObj->m_isSpawnTriggered || eObj->m_isTouchTriggered)
		ImGui::Checkbox("Multi Trigger", &eObj->m_isMultiTriggered);
}

void drawEasingSettings(EffectGameObject* eObj) {
	ImGui::Combo("Easing Type", reinterpret_cast<int*>(&eObj->m_easingType), easingItems, IM_ARRAYSIZE(easingItems));
	if (static_cast<int>(eObj->m_easingType) > 0 && static_cast<int>(eObj->m_easingType) < 7) {
		if (ImGui::InputFloat("Easing Rate", &eObj->m_easingRate, 0.f, 0.f, "%.2f")) {
			if (eObj->m_easingRate < 0.1f) eObj->m_easingRate = 0.1f;
			if (eObj->m_easingRate > 20.f) eObj->m_easingRate = 20.f;
		}
	}
}





void drawStartPosSettings(GameObject* obj) {
	ImGui::Text("StartPos");

	auto spObj = static_cast<StartPosObject*>(obj);
	

}

void drawColorSettings(GameObject* obj) {
	ImGui::Text("Color");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawMoveSettings(GameObject* obj) {
	ImGui::Text("Move");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawStopSettings(GameObject* obj) {
	ImGui::Text("Stop");

	auto tcObj = static_cast<TriggerControlGameObject*>(obj);
	//tcObj->targetID


	drawTouchSpawnTriggered(tcObj);
}

void drawPulseSettings(GameObject* obj) {
	ImGui::Text("Pulse");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawAlphaSettings(GameObject* obj) {
	ImGui::Text("Alpha");

	auto lel = GameManager::sharedState()->m_levelEditorLayer;
	auto eObj = static_cast<EffectGameObject*>(obj);


	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		lel->updateObjectLabel(obj);
	}
	if (ImGui::InputFloat("Fade Time", &eObj->m_duration, 0.f, 0.f, "%.2f")) {
		if (eObj->m_duration < 0.f) eObj->m_duration = 0.f;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;

		//lel->resetEffectTriggerOptim(obj, lel->m_editorUI->m_selectedObjects); fuck you RobTop. Fuck you and your damn inline funcs. I fucking hate you, dickhead.
	}
	if (ImGui::InputFloat("Opacity", &eObj->m_opacity,	0.f, 0.f, "%.2f")) {
		if (eObj->m_opacity < 0.f) eObj->m_opacity = 0.f;
		if (eObj->m_opacity > 1.f) eObj->m_opacity = 1.f;
	}	
	
	drawTouchSpawnTriggered(eObj);
}

void drawToggleSettings(GameObject* obj) {
	if (obj->m_objectID == 1049)
		ImGui::Text("Toggle");
	else if (obj->m_objectID == 1812)
		ImGui::Text("On Death");

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
	ImGui::Text("Spawn");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawRotateSettings(GameObject* obj) {
	ImGui::Text("Rotate");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawScaleSettings(GameObject* obj) {
	ImGui::Text("Scale");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawAnimateSettings(GameObject* obj) {
	ImGui::Text("Animate");

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
	ImGui::Text("Touch");

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawVisibilityLink(GameObject* obj) {
	ImGui::Text("Visibility Link");

	auto eObj = static_cast<EffectGameObject*>(obj);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}
}

void drawReset(GameObject* obj) {
	ImGui::Text("Reset");

	auto eObj = static_cast<EffectGameObject*>(obj);
	if (ImGui::InputInt("Group ID", &eObj->m_targetGroupID)) {
		if (eObj->m_targetGroupID < 0) eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
	}
	drawTouchSpawnTriggered(eObj);
}

void drawUISettings(GameObject* obj) {
	ImGui::Text("UI Settings");

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
	ImGui::Text("TimeWarp");
	
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





void renderObjectSettings(GameObject* obj) {
	int objId = obj->m_objectID;
	if (triggerSet.contains(objId)) {
		if (triggersMap[objId]) {
			ErGuiSettingsDrawer drawTriggerSettings = triggersMap[objId];
			(*drawTriggerSettings)(obj);
		}
		else {
			ImGui::Text("Unknown trigger object...");
		}
	}
	else {
		ImGui::Text("This object is not supported yet...");
	}

	if (ImGui::CollapsingHeader("-----| LEGACY SETTINGS |-----")) {
		if (ImGui::Button("Edit Object")) {
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObject(nullptr);
		}
		if (ImGui::Button("Edit Special")) {
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObjectSpecial(0);
		}
	}
}

void renderMultiObjectSettings(CCArray* objArr) {
	ImGui::Text("MultiObject is not supported yet...");
	if (ImGui::CollapsingHeader("-----| LEGACY SETTINGS |-----")) {
		if (ImGui::Button("Edit Object")) {
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObject(nullptr);
		}
		if (ImGui::Button("Edit Special")) {
			GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObjectSpecial(0);
		}
	}
}

void ErGui::renderEditObjectModule() {
	ImGui::Begin("EditObject-Module");
	auto editorUI = GameManager::sharedState()->getEditorLayer()->m_editorUI;
	if (auto obj = editorUI->m_selectedObject) {
		renderObjectSettings(obj);
	}
	else if (auto objArr = editorUI->m_selectedObjects) {
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

}


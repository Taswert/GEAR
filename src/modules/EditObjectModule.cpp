#pragma once
#include "EditObjectModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"

std::set<int> triggerSet = { 31, 899, 901, 1616, 1006, 1007, 1049, 1268, 1346, 2067, 1347, 1520, 1585, 3033, 1814, 3016, 3660, 3661, 3032, 3006, 3007, 3008, 3009, 3010, 3011, 3012, 3013, 3014, 3015, 3024, 3029, 3030, 3031, 1595, 1611, 1811, 1817, 3614, 3615, 3617, 3619, 3620, 3641, 1912, 2068, 3607, 3608, 3618, 1913, 1914, 1916, 2901, 2015, 2062, 2925, 2016, 1917, 2900, 1934, 3605, 3602, 3603, 3604, 1935, 2999, 3606, 3612, 1615, 3613, 3662, 1815, 3609, 3640, 1816, 3643, 1812, 33, 32, 1613, 1612, 1818, 1819, 3600, 1932, 2899, 3642, 2903, 2066, 3022, 2904, 2905, 2907, 2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919, 2920, 2921, 2922, 2923, 2924, 22, 24, 23, 25, 26, 27, 28, 55, 56, 57, 58, 59, 1915, 3017, 3018, 3019, 3020, 3021, 3023, 29, 30, 105, 744, 915, 1931, 3655, };
using ErGuiSettingsDrawer = void (*)(GameObject*);
std::unordered_map<int, ErGuiSettingsDrawer> triggersMap;

void drawTouchSpawnTriggered(EffectGameObject* eObj) {
	if (eObj->m_isSpawnTriggered) eObj->m_isTouchTriggered = false;
	ImGui::Checkbox("Touch Trigger", &eObj->m_isTouchTriggered);
	if (eObj->m_isTouchTriggered) eObj->m_isSpawnTriggered = false;
	ImGui::Checkbox("Spawn Trigger", &eObj->m_isSpawnTriggered);
	if (eObj->m_isSpawnTriggered || eObj->m_isTouchTriggered)
		ImGui::Checkbox("Multi Trigger", &eObj->m_isMultiTriggered);
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
	if (ImGui::InputFloat("Opacity",	&eObj->m_opacity,	0.f, 0.f, "%.2f")) {
		if (eObj->m_opacity < 0.f) eObj->m_opacity = 0.f;
		if (eObj->m_opacity > 1.f) eObj->m_opacity = 1.f;
	}	
	
	drawTouchSpawnTriggered(eObj);
}

void drawToggleSettings(GameObject* obj) {
	ImGui::Text("Toggle");

	auto eObj = static_cast<EffectGameObject*>(obj);
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

	auto eObj = static_cast<EffectGameObject*>(obj);
	drawTouchSpawnTriggered(eObj);
}

void drawTouchSettings(GameObject* obj) {
	ImGui::Text("Touch");

	auto eObj = static_cast<EffectGameObject*>(obj);
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

	ImGui::Text("-----| LEGACY SETTINGS |-----");
	if (ImGui::Button("Edit Object")) {
		GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObject(nullptr);
	}
	if (ImGui::Button("Edit Special")) {
		GameManager::sharedState()->m_levelEditorLayer->m_editorUI->editObjectSpecial(0);
	}
}

void ErGui::setupTriggersSettings() {
	triggersMap[31] = drawStartPosSettings;
	triggersMap[899] = drawColorSettings;
	triggersMap[901] = drawMoveSettings;
	triggersMap[1616] = drawStopSettings;
	triggersMap[1006] = drawPulseSettings;
	triggersMap[1007] = drawAlphaSettings;
	triggersMap[1049] = drawToggleSettings;
	triggersMap[1268] = drawSpawnSettings;
	triggersMap[1346] = drawRotateSettings;
	triggersMap[2067] = drawScaleSettings;
	triggersMap[1585] = drawAnimateSettings;
	triggersMap[1595] = drawTouchSettings;
}

void ErGui::renderEditObjectModule() {
	ImGui::Begin("EditObject-Module");
	auto editorUI = GameManager::sharedState()->getEditorLayer()->m_editorUI;
	if (auto obj = editorUI->m_selectedObject) {
		renderObjectSettings(obj);
	}
	else {
		ImGui::Text("Object is not selected...");
	}
	ImGui::End();
}
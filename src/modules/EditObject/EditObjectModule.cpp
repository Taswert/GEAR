#pragma once
#include "EditObjectModule.hpp"
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"
#include "EditColorModule.hpp"

#include "AreaTriggers.hpp"
#include "CameraTriggers.hpp"
#include "ShaderTriggers.hpp"
#include "EnterTriggers.hpp"
#include "GroundTriggers.hpp"
#include "SongTriggers.hpp"
#include "StandardTriggers.hpp"
#include "OtherObjects.hpp"
#include "FollowTriggers.hpp"
#include "TimeAndKeyframeTriggers.hpp"
#include "CountAndItemTriggers.hpp"
#include "CollisionTriggers.hpp"

using namespace ErGui;


void ErGui::drawTouchSpawnTriggered(EffectGameObject* eObj) {
	ImGui::Checkbox("Touch Trigger", &eObj->m_isTouchTriggered);
	ImGui::Checkbox("Spawn Trigger", &eObj->m_isSpawnTriggered);

	ImGui::BeginDisabled(!(eObj->m_isSpawnTriggered || eObj->m_isTouchTriggered));
	ImGui::Checkbox("Multi Trigger", &eObj->m_isMultiTriggered);
	ImGui::EndDisabled();
}

void ErGui::drawEasingSettings(EffectGameObject* eObj, float itemsWidth) {
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

bool ErGui::isOldColorTrigger(GameObject* obj) {
	if (oldColorTriggers.contains(obj->m_objectID)) return true;
	else return false;
}

void ErGui::SeparatorPlus(const char* txt) {
	ImGui::Dummy(ImVec2(0.f, dummy.x));
	ImGui::SeparatorText(txt);
	ImGui::Dummy(ImVec2(0.f, dummy.y));

	//if (dummy.x < 0.f) dummy.x = 0.f;
	//if (dummy.y < 0.f) dummy.y = 0.f;
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
			objId = 0;
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
	
	if (objId != 0) {
		ErGuiSettingsDrawer drawTriggerSettings = triggersMap[objId];
		(*drawTriggerSettings)(static_cast<GameObject*>(objArr->objectAtIndex(0)), objArr);
	}
	else {
		ImGui::Text("Not Compatible...");
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
	// StandardTriggers.hpp
	mapStandardTriggers();
	mapOldColorTriggers();

	// AreaTriggers.hpp
	mapAreaTriggers(); // ToDo

	// CameraTriggers.hpp
	mapCameraTriggers();

	// Shaders.hpp
	mapShaderTriggers(); // ToDo

	// EnterTriggers.hpp
	mapEnterTriggers(); // ToDo

	// GroundTriggers.hpp
	mapGroundTriggers(); // ToDo

	// SongTriggers.hpp
	mapSongTriggers(); // ToDo... Oh...

	// OtherObjects.hpp
	mapOtherTriggers();
	mapOtherObjects();

	// FollowTriggers.hpp
	mapFollowTriggers(); // ToDo

	// TimeAndKeyframeTriggers.hpp
	mapTimeAndKeyframeTriggers(); // ToDo

	// CountAndItemTriggers.hpp
	mapCountAndItemTriggers(); // ToDo

	// CollisionTriggers.hpp
	mapCollisionTriggers();
}


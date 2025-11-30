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
	static int localSelectVersion = -1;
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


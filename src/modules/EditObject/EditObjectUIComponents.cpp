#include "EditObjectUIComponents.hpp"
#include "EditObject/EditObjectModule.hpp"

void rawDrawTargetGroupID(EffectGameObject* eObj, CCArray* objArr, const char* targetGroupLabel) {
	ImGui::Text(targetGroupLabel);
	ImGui::SameLine(80.f);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::BetterDragInt(fmt::format("##{}", targetGroupLabel).c_str(), &eObj->m_targetGroupID, 1, 5, "%d", 1, 0, 9999, ImGuiSliderFlags_ClampZeroRange)) {
		if (eObj->m_targetGroupID < 0)	eObj->m_targetGroupID = 0;
		if (eObj->m_targetGroupID > 9999) eObj->m_targetGroupID = 9999;
		auto targetGroup = eObj->m_targetGroupID;
		auto lel = LevelEditorLayer::get();
		lel->updateObjectLabel(eObj);
		APPLY_FIELDS_TO_OTHER_TRIGGERS_AND_UPDATE(m_targetGroupID, targetGroup, EffectGameObject);
	}
}

void rawDrawCenterGroupID(EffectGameObject* eObj, CCArray* objArr, const char* centerGroupLabel) {
	ImGui::Text(centerGroupLabel);
	ImGui::SameLine(80.f);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::BetterDragInt(fmt::format("##{}", centerGroupLabel).c_str(), &eObj->m_centerGroupID, 1, 5, "%d", 1, 0, 9999, ImGuiSliderFlags_ClampZeroRange)) {
		if (eObj->m_centerGroupID < 0)	eObj->m_centerGroupID = 0;
		if (eObj->m_centerGroupID > 9999) eObj->m_centerGroupID = 9999;
		auto centerGroup = eObj->m_centerGroupID;
		auto lel = LevelEditorLayer::get();
		lel->updateObjectLabel(eObj);
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_centerGroupID, centerGroup, EffectGameObject);
	}
}

void ErGui::drawComponentGroupID(EffectGameObject* eObj, CCArray* objArr, const char* targetGroupLabel, const char* centerGroupLabel) {
	ImGui::SeparatorText("Group Settings");
	rawDrawTargetGroupID(eObj, objArr, targetGroupLabel);
	rawDrawTargetGroupID(eObj, objArr, centerGroupLabel);
}

void ErGui::drawComponentGroupID(EffectGameObject* eObj, CCArray* objArr, const char* targetGroupLabel) {
	ImGui::SeparatorText("Group Settings");
	rawDrawTargetGroupID(eObj, objArr, targetGroupLabel);
}

void rawDrawEasingSettings(EffectGameObject* eObj, CCArray* objArr) {
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ImGui::Combo("Easing Type", reinterpret_cast<int*>(&eObj->m_easingType), ErGui::easingItems, IM_ARRAYSIZE(ErGui::easingItems))) {
		auto field = eObj->m_easingType;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_easingType, field, EffectGameObject);
	}
	if (static_cast<int>(eObj->m_easingType) > 0 && static_cast<int>(eObj->m_easingType) < 7) {
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		if (ErGui::BetterDragFloat("Easing Rate", &eObj->m_easingRate, 0.1f, 0.5f, "%.2f", 0.05f, 0.1f, 20.f)) {
			auto field = eObj->m_easingRate;
			if (field < 0.1f) eObj->m_easingRate = 0.1f;
			if (field > 20.f) eObj->m_easingRate = 20.f;
			field = eObj->m_easingRate;
			APPLY_FIELDS_TO_OTHER_TRIGGERS(m_easingRate, field, EffectGameObject);
		}
	}
}

void ErGui::drawEasingSettings(EffectGameObject* eObj, CCArray* objArr) {
	ImGui::SeparatorText("Easing");

	rawDrawEasingSettings(eObj, objArr);
}

void ErGui::drawComponentTime(EffectGameObject* eObj, CCArray* objArr, const char* label, bool enableEasing) {
	ImGui::SeparatorText("Time");

	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
	if (ErGui::BetterDragFloat(label, &eObj->m_duration, 0.1f, 0.5f, "%.2f", 0.05f)) {
		if (eObj->m_duration < -1.f) eObj->m_duration = -1.f;
		auto duration = eObj->m_duration;
		auto somePoint = reinterpret_cast<CCPoint*>(geode::base::get() + 0x6a40b8);
		eObj->m_endPosition = *somePoint;

		for (auto objInArr : CCArrayExt<EnhancedTriggerObject*>(objArr)) {
			objInArr->m_duration = duration;
			objInArr->m_endPosition = *somePoint;
		}
	}

	if (enableEasing) 
		rawDrawEasingSettings(eObj, objArr);
}

void ErGui::drawTouchSpawnTriggered(EffectGameObject* eObj, CCArray* objArr) {
	ImGui::SeparatorText("Trigger Type");

	if (ImGui::Checkbox("Touch Trigger", &eObj->m_isTouchTriggered)) {
		auto field = eObj->m_isTouchTriggered;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isTouchTriggered, field, EffectGameObject);
	}
	if (ImGui::Checkbox("Spawn Trigger", &eObj->m_isSpawnTriggered)) {
		auto field = eObj->m_isSpawnTriggered;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isSpawnTriggered, field, EffectGameObject);
	}

	ImGui::BeginDisabled(!(eObj->m_isSpawnTriggered || eObj->m_isTouchTriggered));
	if (ImGui::Checkbox("Multi Trigger", &eObj->m_isMultiTriggered)) {
		auto field = eObj->m_isMultiTriggered;
		APPLY_FIELDS_TO_OTHER_TRIGGERS(m_isMultiTriggered, field, EffectGameObject);
	}
	ImGui::EndDisabled();
}


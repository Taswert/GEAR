#pragma once
#include "SelectFilterModule.hpp"

void ErGui::renderSelectFilter() {
	ImGui::Begin("Filter");
	ImGui::PushStyleColor(ImGuiCol_Separator, { 0.33f, 0.33f, 0.33f, 1.f });

	if (ImGui::CollapsingHeader("Main Settings")) {
		ImGui::Checkbox("Object", &filterByObjects);
		ImGui::SameLine(120.f);
		ImGui::Checkbox("Group", &filterByGroups);

		ImGui::Checkbox("MainColor", &filterByMainColors);
		ImGui::SameLine(120.f);
		ImGui::Checkbox("DetailColor", &filterByDetailColors);

		ImGui::Checkbox("Channel", &filterByChannel);
		ImGui::SameLine(120.f);
		ImGui::Checkbox("ObjectType", &filterByType);

		ImGui::Dummy({0.f, 3.f});
		ImGui::SeparatorText("Modifier");
		ImGui::RadioButton("Or", &filterModifier, 0);
		ImGui::SameLine(120.f);
		ImGui::RadioButton("And", &filterModifier, 1);
	}

	if (ImGui::CollapsingHeader("Object Filter")) {
		if (GameObject* obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject) {
			ImGui::Text("ObjectID: %d", obj->m_objectID);
			ImGui::SameLine();
			if (ImGui::Button("Add##OBJ")) {
				objectsFilterSet.insert(obj->m_objectID);
			}


			auto bc = obj->m_baseColor;
			auto dc = obj->m_detailColor;
			if (bc) ImGui::Text("MainColorID: %d", bc->m_colorID);
			if (dc) ImGui::Text("DetailColorID: %d", dc->m_colorID);

		}
		else {
			ImGui::Text("ObjectID: -");
		}

		if (GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects->count() > 0) {
			auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;

			//std::string objectsIdString = "ObjectIDs: ";
			//ImGui::Text(objectsIdString.c_str());

			if (ImGui::Button("Add All##OBJ")) {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					objectsFilterSet.insert(obj->m_objectID);
				}
			}
		}
		if (ImGui::Button("Reset##OBJ")) {
			objectsFilterSet.clear();
		}

		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto objId : objectsFilterSet) {
			std::string btnStr = std::to_string(objId);
			btnStr += "##OBJ-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				objectsFilterSet.erase(objId);
			}
			if (i % 10 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 10 != 0) ImGui::NewLine();
	}


	if (ImGui::CollapsingHeader("Main Color Filter")) {
		ImGui::Text("Offset");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##MCOL-Offset", &offsetMainColorSFM);
		setMin(offsetMainColorSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Next Free##MCOL")) {
			for (int i = offsetMainColorSFM; i < 10000; i++) {
				if (!mainColorsFilterSet.contains(i)) {
					chosenMainColorSFM = i;
					break;
				}
			}
		}

		ImGui::Text("ColorID");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##MCOL-ColorID", &chosenMainColorSFM);
		setMin(chosenMainColorSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Add##MCOL") && chosenMainColorSFM > 0) {
			mainColorsFilterSet.insert(chosenMainColorSFM);
		}
		if (ImGui::Button("Reset##MCOL")) {
			mainColorsFilterSet.clear();
		}
	
		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto colId : mainColorsFilterSet) {
			std::string btnStr = std::to_string(colId);
			btnStr += "##MCOL-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				mainColorsFilterSet.erase(colId);
			}
			if (i % 10 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 10 != 0) ImGui::NewLine();
	}
	
	
	if (ImGui::CollapsingHeader("Detail Color Filter")) {
		ImGui::Text("Offset");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##DCOL-Offset", &offsetDetailColorSFM);
		setMin(offsetDetailColorSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Next Free##DCOL")) {
			for (int i = offsetDetailColorSFM; i < 1000; i++) {
				if (!detailColorsFilterSet.contains(i)) {
					chosenDetailColorSFM = i;
					break;
				}
			}
		}

		ImGui::Text("ColorID");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##DCOL-ColorID", &chosenDetailColorSFM);
		setMin(chosenDetailColorSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Add##DCOL") && chosenDetailColorSFM > 0) {
			detailColorsFilterSet.insert(chosenDetailColorSFM);
		}
		if (ImGui::Button("Reset##DCOL")) {
			detailColorsFilterSet.clear();
		}
	
		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto colId : detailColorsFilterSet) {
			std::string btnStr = std::to_string(colId);
			btnStr += "##DCOL-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				detailColorsFilterSet.erase(colId);
			}
			if (i % 10 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 10 != 0) ImGui::NewLine();
	}


	if (ImGui::CollapsingHeader("GroupID Filter")) {
		ImGui::Text("Offset");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##GROUP-Offset", &offsetGroupSFM);
		setMin(offsetGroupSFM, 1);
		ImGui::SameLine();
		if (ImGui::Button("Next Free##GROUP")) {
			for (int i = offsetGroupSFM; i < 1000; i++) {
				if (!groupsFilterSet.contains(i)) {
					chosenGroupSFM = i;
					break;
				}
			}
		}

		ImGui::Text("GroupID");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##GROUP-ID", &chosenGroupSFM);
		setMin(chosenGroupSFM, 1);
		ImGui::SameLine();
		if (ImGui::Button("Add##GROUP") && chosenGroupSFM > 0) {
			groupsFilterSet.insert(chosenGroupSFM);
		}
		if (ImGui::Button("Reset##GROUP")) {
			groupsFilterSet.clear();
		}

		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto grpId : groupsFilterSet) {
			std::string btnStr = std::to_string(grpId);
			btnStr += "##GROUP-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				groupsFilterSet.erase(grpId);
			}
			if (i % 10 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 10 != 0) ImGui::NewLine();
	}
	

	if (ImGui::CollapsingHeader("Channel Filter")) {
		ImGui::Text("Offset");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##CHAN-Offset", &offsetChannelSFM);
		setMin(offsetChannelSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Next Free##CHAN")) {
			for (int i = offsetChannelSFM; i < 10000; i++) {
				if (!channelFilterSet.contains(i)) {
					chosenChannelSFM = i;
					break;
				}
			}
		}
		
		ImGui::Text("ChannelID");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputInt("##CHAN-ID", &chosenChannelSFM);
		setMin(chosenChannelSFM, 0);
		ImGui::SameLine();
		if (ImGui::Button("Add##CHAN") && chosenChannelSFM > -1) {
			channelFilterSet.insert(chosenChannelSFM);
		}
		if (ImGui::Button("Reset##CHAN")) {
			channelFilterSet.clear();
		}

		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto chnlId : channelFilterSet) {
			std::string btnStr = std::to_string(chnlId);
			btnStr += "##CHAN-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				channelFilterSet.erase(chnlId);
			}
			if (i % 10 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 10 != 0) ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("Object Type Filter")) {
		ImGui::Text("Object Type");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::Combo("##OBJECT-TYPE", &chosenTypeSFM, objectTypes, IM_ARRAYSIZE(objectTypes));
		ImGui::SameLine();
		if (ImGui::Button("Add##OBJTYPE") && chosenTypeSFM > -1) {
			objectTypeSet.insert(chosenTypeSFM);
		}
		if (ImGui::Button("Reset##OBJTYPE")) {
			objectTypeSet.clear();
		}

		ImGui::SeparatorText("Filter");
		int i = 1;
		for (auto objType : objectTypeSet) {
			std::string btnStr = objectTypes[objType];
			btnStr += "##OBJTYPE-FILTER";
			if (ImGui::Button(btnStr.c_str())) {
				objectTypeSet.erase(objType);
			}
			if (i % 2 != 0) ImGui::SameLine();
			i++;
		}
		if (i - 1 % 2 != 0) ImGui::NewLine();
	}

	ImGui::PopStyleColor();
	ImGui::End();
}
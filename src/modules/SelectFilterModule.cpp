#pragma once
#include "SelectFilterModule.hpp"
bool ErGui::selectFilterRealization(GameObject* obj) {
	bool isFilterOn = false;
	bool shouldSelect = false;

	if (!ErGui::objectsFilterSet.empty() && ErGui::filterByObjects) {
		isFilterOn = true;
		if (ErGui::objectsFilterSet.contains(obj->m_objectID)) {
			shouldSelect = true;
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::mainColorsFilterSet.empty() && ErGui::filterByMainColors) {
		isFilterOn = true;
		if (obj->m_baseColor) {
			if (ErGui::mainColorsFilterSet.contains(obj->m_baseColor->m_colorID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::detailColorsFilterSet.empty() && ErGui::filterByDetailColors) {
		isFilterOn = true;
		if (obj->m_detailColor) {
			if (ErGui::detailColorsFilterSet.contains(obj->m_detailColor->m_colorID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::groupsFilterSet.empty() && ErGui::filterByGroups) {
		isFilterOn = true;
		if (obj->m_groups) {
			bool foundGroup = false;
			for (int i = 0; i < obj->m_groups->size(); i++) {
				if (ErGui::groupsFilterSet.contains(obj->m_groups->at(i))) {
					shouldSelect = true;
					foundGroup = true;
				}
			}
			if (!foundGroup && ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}

	if (!ErGui::channelFilterSet.empty() && ErGui::filterByChannel) {
		isFilterOn = true;
		if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
			if (ErGui::channelFilterSet.contains(egObj->m_channelValue)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				return false;
			}
		}
		else if (ErGui::filterModifier == 1) {
			return false;
		}
	}


	if (!isFilterOn)
		return true; // EditorUI::selectObject(obj, p1);
	else if (shouldSelect)
		return true; // EditorUI::selectObject(obj, p1);
	else
		return false;
}

CCArray* ErGui::selectFilterRealization(CCArray* objArrInRect) {

	CCArray* shouldSelectArr = CCArray::create();
	for (auto obj : CCArrayExt<GameObject*>(objArrInRect)) {

		bool isFilterOn = false;
		bool shouldSelect = false;

		if (!ErGui::objectsFilterSet.empty() && ErGui::filterByObjects) {
			isFilterOn = true;
			if (ErGui::objectsFilterSet.contains(obj->m_objectID)) {
				shouldSelect = true;
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::mainColorsFilterSet.empty() && ErGui::filterByMainColors) {
			isFilterOn = true;
			if (obj->m_baseColor) {
				if (ErGui::mainColorsFilterSet.contains(obj->m_baseColor->m_colorID)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::detailColorsFilterSet.empty() && ErGui::filterByDetailColors) {
			isFilterOn = true;
			if (obj->m_detailColor) {
				if (ErGui::detailColorsFilterSet.contains(obj->m_detailColor->m_colorID)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::groupsFilterSet.empty() && ErGui::filterByGroups) {
			isFilterOn = true;
			if (obj->m_groups) {
				bool foundGroup = false;
				for (int i = 0; i < obj->m_groups->size(); i++) {
					if (ErGui::groupsFilterSet.contains(obj->m_groups->at(i))) {
						std::cout << "ye\n";
						shouldSelect = true;
						foundGroup = true;
					}
				}
				if (!foundGroup && ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}

		if (!ErGui::channelFilterSet.empty() && ErGui::filterByChannel) {
			isFilterOn = true;
			if (auto egObj = dynamic_cast<EffectGameObject*>(obj)) {
				if (ErGui::channelFilterSet.contains(egObj->m_channelValue)) {
					shouldSelect = true;
				}
				else if (ErGui::filterModifier == 1) {
					continue;
				}
			}
			else if (ErGui::filterModifier == 1) {
				continue;
			}
		}


		if (!isFilterOn)
			return objArrInRect;
		else if (shouldSelect)
			shouldSelectArr->addObject(obj);
		else
			continue;
	}

	return shouldSelectArr;
}

void ErGui::renderSelectFilter() {
	ImGui::Begin("SelectFilter-Module");
	ErGui::enableClicks();


	if (ImGui::CollapsingHeader("Main Settings")) {
		ImGui::Checkbox("Object", &filterByObjects);
		ImGui::SameLine(120.f);
		ImGui::Checkbox("Group", &filterByGroups);

		ImGui::Checkbox("MainColor", &filterByMainColors);
		ImGui::SameLine(120.f);
		ImGui::Checkbox("DetailColor", &filterByDetailColors);

		ImGui::Checkbox("Channel", &filterByChannel);


		ImGui::Text("---| Modifier |---");
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


		ImGui::Text("-----| Filter |-----");
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
		ImGui::InputInt("ColorID##MCOL", &chosenMainColorSFM);
		setMin(chosenMainColorSFM, 0);
		ImGui::InputInt("Offset##MCOL", &offsetMainColorSFM);
		setMin(offsetMainColorSFM, 0);
		if (ImGui::Button("Add##MCOL") && chosenMainColorSFM > 0) {
			mainColorsFilterSet.insert(chosenMainColorSFM);
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##MCOL")) {
			for (int i = offsetMainColorSFM; i < 10000; i++) {
				if (!mainColorsFilterSet.contains(i)) {
					chosenMainColorSFM = i;
					break;
				}
			}
		}
	
		ImGui::Text("-----| Filter |-----");
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
		ImGui::InputInt("ColorID##DCOL", &chosenDetailColorSFM);
		setMin(chosenDetailColorSFM, 0);
		ImGui::InputInt("Offset##DCOL", &offsetDetailColorSFM);
		setMin(offsetDetailColorSFM, 0);
		if (ImGui::Button("Add##DCOL") && chosenDetailColorSFM > 0) {
			detailColorsFilterSet.insert(chosenDetailColorSFM);
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##DCOL")) {
			for (int i = offsetDetailColorSFM; i < 1000; i++) {
				if (!detailColorsFilterSet.contains(i)) {
					chosenDetailColorSFM = i;
					break;
				}
			}
		}
	
		ImGui::Text("-----| Filter |-----");
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
		ImGui::InputInt("GroupID", &chosenGroupSFM);
		setMin(chosenGroupSFM, 1);
		ImGui::InputInt("Offset##GROUP", &offsetGroupSFM);
		setMin(offsetGroupSFM, 1);
		if (ImGui::Button("Add##GROUP") && chosenGroupSFM > 0) {
			groupsFilterSet.insert(chosenGroupSFM);
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##GROUP")) {
			for (int i = offsetGroupSFM; i < 1000; i++) {
				if (!groupsFilterSet.contains(i)) {
					chosenGroupSFM = i;
					break;
				}
			}
		}

		ImGui::Text("-----| Filter |-----");
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
		ImGui::InputInt("ChannelID", &chosenChannelSFM);
		setMin(chosenChannelSFM, 0);
		ImGui::InputInt("Offset##CHAN", &offsetChannelSFM);
		setMin(offsetChannelSFM, 0);
		if (ImGui::Button("Add##CHAN") && chosenChannelSFM > -1) {
			channelFilterSet.insert(chosenChannelSFM);
		}
		ImGui::SameLine();
		if (ImGui::Button("Next Free##CHAN")) {
			for (int i = offsetChannelSFM; i < 10000; i++) {
				if (!channelFilterSet.contains(i)) {
					chosenChannelSFM = i;
					break;
				}
			}
		}

		ImGui::Text("-----| Filter |-----");
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

	ImGui::End();
}
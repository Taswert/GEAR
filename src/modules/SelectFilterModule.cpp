#pragma once
#include "SelectFilterModule.hpp"

void ErGui::renderSelectFilter() {
	ImGui::Begin("SelectFilter-Module");

	ImGui::Checkbox("Object Filter", &filterByObjects);
	ImGui::Checkbox("Color Filter", &filterByMainColors);
	ImGui::Checkbox("Group Filter", &filterByGroups);
	ImGui::Checkbox("Or Modifier", &filterOr);

	if (GameObject* obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject) {
		ImGui::Text("ObjectID: %d", obj->m_objectID);
		ImGui::SameLine();
		if (ImGui::Button("Add##OBJ")) {
			bool foundSame = false;
			for (int i = 0; i < objectsFilterArr.size(); i++) {
				if (objectsFilterArr[i] == obj->m_objectID) {
					foundSame = true;
					break;
				}
			}
			if (!foundSame) objectsFilterArr.push_back(obj->m_objectID);
		}

	}
	else {
		ImGui::Text("ObjectID: -");
	}

	if (GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects->count() > 0) {
		auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;
		std::string objectsIdString = "ObjectIDs: ";

		std::vector<int> selectedObjsArr;

		//for (int i = 0; i < objArr->count(); i++) {
		//	bool foundSame = false;
		//	for (int j = 0; j < selectedObjsArr.size(); j++) {
		//		if (selectedObjsArr[j] == static_cast<GameObject*>(objArr->objectAtIndex(i))->m_objectID) {
		//			foundSame = true;
		//			break;
		//		}
		//	}
		//	if (!foundSame) selectedObjsArr.push_back(static_cast<GameObject*>(objArr->objectAtIndex(i))->m_objectID);
		//}

		//for (int i = 0; i < selectedObjsArr.size(); i++) {
		//	objectsIdString += std::to_string(selectedObjsArr[i]);
		//	objectsIdString += ", ";
		//}

		ImGui::Text(objectsIdString.c_str());
	}


	ImGui::InputInt("ColorID", &chosenColorSFM);
	if (ImGui::Button("Add##COL")) {
		bool foundSame = false;
		for (int i = 0; i < mainColorsFilterArr.size(); i++) {
			if (mainColorsFilterArr[i] == chosenColorSFM) {
				foundSame = true;
				break;
			}
		}
		if (!foundSame) mainColorsFilterArr.push_back(chosenColorSFM);
	}

	ImGui::InputInt("GroupID", &chosenGroupSFM);
	if (ImGui::Button("Add##GRP")) {
		bool foundSame = false;
		for (int i = 0; i < groupsFilterArr.size(); i++) {
			if (groupsFilterArr[i] == chosenGroupSFM) {
				foundSame = true;
				break;
			}
		}
		if (!foundSame) groupsFilterArr.push_back(chosenGroupSFM);
	}

	ImGui::Text("-----| Objects Filter |-----");
	for (int i = 0; i < objectsFilterArr.size(); i++) {
		ImGui::Text("%d", objectsFilterArr[i]);
		ImGui::SameLine();
		std::string btnStr = "Remove##OBJ";
		btnStr += std::to_string(objectsFilterArr[i]);
		if (ImGui::Button(btnStr.c_str())) {
			objectsFilterArr.erase(objectsFilterArr.begin() + i);
		}
	}

	ImGui::Text("-----| Main Colors Filter |-----");
	for (int i = 0; i < mainColorsFilterArr.size(); i++) {
		ImGui::Text("%d", mainColorsFilterArr[i]);
		ImGui::SameLine();
		std::string btnStr = "Remove##COL";
		btnStr += std::to_string(mainColorsFilterArr[i]);
		if (ImGui::Button(btnStr.c_str())) {
			mainColorsFilterArr.erase(mainColorsFilterArr.begin() + i);
		}
	}

	ImGui::Text("-----| Groups Filter |-----");
	for (int i = 0; i < groupsFilterArr.size(); i++) {
		ImGui::Text("%d", groupsFilterArr[i]);
		ImGui::SameLine();
		std::string btnStr = "Remove##GRP";
		btnStr += std::to_string(groupsFilterArr[i]);
		if (ImGui::Button(btnStr.c_str())) {
			groupsFilterArr.erase(groupsFilterArr.begin() + i);
		}
	}
	ImGui::End();
}
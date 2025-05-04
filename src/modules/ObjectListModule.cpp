#pragma once
#include "ObjectListModule.hpp"
#include <Geode/Geode.hpp>
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"

using namespace geode::prelude;

float buttonSizeValue = 30.f;
std::pair<const char*, std::vector<ErGui::ObjectConfig>> favouritesObjects = {
	"Favourites", {
		{0, {}}
	}
};


bool ImageButtonFromFrameName(ErGui::ObjectConfig& objCfg, int j, ImGuiTextFilter filter, const char* str_id, ImVec2 imageSize = ImVec2(30.f, 30.f), ImVec4 bgCol = ImVec4(0, 0, 0, 0), ImVec4 tintCol = ImVec4(1, 1, 1, 1)) {
	int objId = objCfg.objectIdVector[j];
	std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(objId);
	if (newFrameName.empty()) return false;
	CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
	if (!frame) return false;

	EditorUI* editorUI = GameManager::sharedState()->m_levelEditorLayer->m_editorUI;


	//Color for selected object
	bool shouldPopStyle = false;
	if (editorUI->m_selectedObjectIndex == objId) {
		ImGui::PushStyleColor(ImGuiCol_Button, {
			ImGui::GetStyleColorVec4(ImGuiCol_Button).x + 0.2f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).y + 0.2f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).z + 0.2f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).w + 0.2f });
		shouldPopStyle = true;
	}

	if (ImGui::Button(str_id, imageSize)) {
		if (editorUI->m_selectedObjectIndex != objId)
			editorUI->m_selectedObjectIndex = objId;
		else
			editorUI->m_selectedObjectIndex = 0;
		editorUI->updateGridNodeSize();
	}
	
	//Pop color for selected object
	if (shouldPopStyle) ImGui::PopStyleColor();

	std::string objSettingsPopupStr = std::string("ObjectSettingsPopup") + str_id;

	if (ImGui::BeginPopupContextItem())
	{
		ImGui::Text("Object Settings");
		if (ImGui::Button("Favourite")) {
			favouritesObjects.second.at(0).objectIdVector.push_back(objId);
		}
		if (ImGui::Button("Set As Thumbnail")) { 
			//This is a new obj confing, so it won't work. Should get address of original config
			auto* pointerObjCfg = &objCfg;
			pointerObjCfg->thumbnailObjectId = objId;
			//ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ErGui::drawImageInImGui(frame);


	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip(newFrameName.c_str());
	}


	return true;
}

void ImageFolderButton(std::vector<ErGui::ObjectConfig> visibleButtons, int i, ImVec2 buttonSize = ImVec2(30.f, 30.f)) {

	int folderId = visibleButtons[i].thumbnailObjectId;
	// Если для этой папки ещё нет записи, то по умолчанию она закрыта

	if (folderId == 0)
		return ;


	std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(folderId);
	if (newFrameName.empty()) 
		return ;

	CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
	if (!frame) 
		return ;


	ImGui::PushStyleColor(ImGuiCol_Button, { 0.4f, 0.f, 0.5f, 1.f });

	std::string folderStrId = "##FOLDER-";
	folderStrId.append(std::to_string(folderId));
	if (ImGui::Button(folderStrId.c_str(), buttonSize)) {
		std::string popupStr = "##POPUP-";
		popupStr.append(std::to_string(folderId));
		ImGui::OpenPopup(popupStr.c_str());
	}

	ImGui::PopStyleColor();

	ErGui::drawImageInImGui(frame);

	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float lastButtonX2 = ImGui::GetItemRectMax().x;
	float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

	if (i + 1 < visibleButtons.size() && nextButtonX2 < windowVisibleX2)
		ImGui::SameLine();

}

void objectTabCreate(const char* name, std::vector<ErGui::ObjectConfig>& mySet, ImGuiTextFilter filter, ImVec2 buttonSize) {

	std::vector<ErGui::ObjectConfig> visibleButtons;
	for (ErGui::ObjectConfig oc : mySet) {
		std::vector<int> visibleObjects = { };
		
		for (int i : oc.objectIdVector) {
			auto frameName = ObjectToolbox::sharedState()->intKeyToFrame(i);
			if (filter.PassFilter(frameName))
				visibleObjects.push_back(i);
		}

		if (!visibleObjects.empty()) {
			// Копируем объект, но заменяем objectIdVector на видимые объекты
			ErGui::ObjectConfig newOc = oc;
			newOc.objectIdVector = visibleObjects;
			visibleButtons.push_back(newOc);
		}
	}

	if (visibleButtons.empty()) 
		return;

	if (ImGui::CollapsingHeader(name)) {
		float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		for (int i = 0; i < visibleButtons.size(); i++) {
			ImageFolderButton(visibleButtons, i, buttonSize);
			std::string popupStr = "##POPUP-";
			popupStr.append(std::to_string(visibleButtons[i].thumbnailObjectId));
			if (visibleButtons[i].thumbnailObjectId != 0 && ImGui::BeginPopup(popupStr.c_str())) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i], j, filter, strId.c_str(), buttonSize);

					if (j + 1 < visibleButtons[i].objectIdVector.size() && (j + 1) % 6 != 0)
						ImGui::SameLine();
				}
				ImGui::EndPopup();
			}
			else if (visibleButtons[i].thumbnailObjectId == 0) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i], j, filter, strId.c_str(), buttonSize);
					//objCount++;

					float lastButtonX2 = ImGui::GetItemRectMax().x;
					float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

					if (j + 1 < visibleButtons[i].objectIdVector.size() && nextButtonX2 < windowVisibleX2)
						ImGui::SameLine();
				}
			}
		}
	}
}

void ErGui::renderObjectList() {
	//ErGui::setupMenuColors(ImVec4(0.05f, 0.10f, 0.02f, 0.95f), ImVec4(0.38f, 0.77f, 0.12f, 1.f), ImVec4(0.63f, 0.80f, 0.49f, 1.f));
	ImGui::Begin("ObjectList-Module");
	static ImGuiTextFilter filter;
	filter.Draw();
	
	ImGui::InputFloat("Button Size", &buttonSizeValue);
	ImVec2 buttonSize(buttonSizeValue, buttonSizeValue);

	for (auto object : ErGui::objectCfg) {
		objectTabCreate(object.first, object.second, filter, buttonSize);
	}

	objectTabCreate(favouritesObjects.first, favouritesObjects.second, filter, buttonSize);

	//for (int i = 0; i < 5; i++) ImGui::PopStyleColor();
	ImGui::End();
}


//Settings!
//Close popup after choosing an object - todo
//Buttons Size - Done
//Favourite objects
//Set Object as Thumbnail button
//Recent???
//...
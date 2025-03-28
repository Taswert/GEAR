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


void drawImage(CCSpriteFrame* frame) {

	CCSprite* tempSprite = CCSprite::createWithSpriteFrame(frame);
	auto quad = tempSprite->getQuad();

	float u_min = std::min({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
	float u_max = std::max({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
	float v_min = std::min({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });
	float v_max = std::max({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });

	auto texture = tempSprite->getTexture();
	GLuint textureID = texture->getName();
	ImTextureID imguiTexture = (ImTextureID)(intptr_t)textureID;


	auto drawList = ImGui::GetWindowDrawList();

	if (tempSprite->getContentWidth() > tempSprite->getContentHeight()) {
		float ratio = tempSprite->getContentWidth() / (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
		float newY = tempSprite->getContentHeight() / ratio;
		float centerY = (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y) / 2.f;

		drawList->AddImageQuad(
			textureID,
			ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y + (newY / 2) - centerY), ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y + (newY / 2) - centerY),
			ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y - (newY / 2) - centerY), ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y - (newY / 2) - centerY),
			ImVec2(quad.bl.texCoords.u, quad.bl.texCoords.v), ImVec2(quad.br.texCoords.u, quad.br.texCoords.v),
			ImVec2(quad.tr.texCoords.u, quad.tr.texCoords.v), ImVec2(quad.tl.texCoords.u, quad.tl.texCoords.v)
		);
	}
	else {
		float ratio = tempSprite->getContentHeight() / (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y);
		float newX = tempSprite->getContentWidth() / ratio;
		float centerX = (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x) / 2.f;

		drawList->AddImageQuad(
			textureID,
			ImVec2(ImGui::GetItemRectMin().x - (newX / 2) + centerX, ImGui::GetItemRectMax().y), ImVec2(ImGui::GetItemRectMin().x + (newX / 2) + centerX, ImGui::GetItemRectMax().y),
			ImVec2(ImGui::GetItemRectMin().x + (newX / 2) + centerX, ImGui::GetItemRectMin().y), ImVec2(ImGui::GetItemRectMin().x - (newX / 2) + centerX, ImGui::GetItemRectMin().y),
			ImVec2(quad.bl.texCoords.u, quad.bl.texCoords.v), ImVec2(quad.br.texCoords.u, quad.br.texCoords.v),
			ImVec2(quad.tr.texCoords.u, quad.tr.texCoords.v), ImVec2(quad.tl.texCoords.u, quad.tl.texCoords.v)
		);
	}
}

bool ImageButtonFromFrameName(int objId, ImGuiTextFilter filter, const char* str_id, ImVec2 imageSize = ImVec2(30.f, 30.f), ImVec4 bgCol = ImVec4(0, 0, 0, 0), ImVec4 tintCol = ImVec4(1, 1, 1, 1)) {

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

	//if (ImGui::ImageButton(str_id, imguiTexture, imageSize, ImVec2(u_min, v_min), ImVec2(u_max, v_max), bgCol, tintCol)) {
	if (ImGui::Button(str_id, imageSize)) {
		if (editorUI->m_selectedObjectIndex != objId)
			editorUI->m_selectedObjectIndex = objId;
		else
			editorUI->m_selectedObjectIndex = 0;
		editorUI->updateGridNodeSize();
		std::cout << "ID: " << objId << "\n" << "Name: " << ObjectToolbox::sharedState()->intKeyToFrame(objId) << "\n";
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
		if (ImGui::Button("Delete"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	drawImage(frame);


	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip(newFrameName.c_str());
	}


	return true;
}

void ImageFolderButton(std::unordered_map<int, bool>& folderOpenStates, std::vector<ErGui::ObjectConfig> visibleButtons, int i, ImVec2 buttonSize = ImVec2(30.f, 30.f)) {

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

	drawImage(frame);

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
		static std::unordered_map<int, bool> folderOpenStates;

		for (int i = 0; i < visibleButtons.size(); i++) {
			ImageFolderButton(folderOpenStates, visibleButtons, i, buttonSize);
			std::string popupStr = "##POPUP-";
			popupStr.append(std::to_string(visibleButtons[i].thumbnailObjectId));
			if (visibleButtons[i].thumbnailObjectId != 0 && ImGui::BeginPopup(popupStr.c_str())) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i].objectIdVector[j], filter, strId.c_str(), buttonSize);

					if (j + 1 < visibleButtons[i].objectIdVector.size() && (j + 1) % 6 != 0)
						ImGui::SameLine();
				}
				ImGui::EndPopup();
			}
			else if (visibleButtons[i].thumbnailObjectId == 0) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i].objectIdVector[j], filter, strId.c_str(), buttonSize);
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
	ImGui::Begin("ObjectList-Module");
	static ImGuiTextFilter filter;
	filter.Draw();
	
	ImGui::InputFloat("Button Size", &buttonSizeValue);
	ImVec2 buttonSize(buttonSizeValue, buttonSizeValue);

	for (auto object : ErGui::getObjectCfg()) {
		objectTabCreate(object.first, object.second, filter, buttonSize);
	}

	objectTabCreate(favouritesObjects.first, favouritesObjects.second, filter, buttonSize);
	
	ImGui::End();
}


//Settings!
//Close popup after choosing an object - todo
//Buttons Size - Done
//Favourite objects
//Set Object as Thumbnail button
//...
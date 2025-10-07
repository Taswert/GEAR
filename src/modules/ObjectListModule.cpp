#pragma once
#include "ObjectListModule.hpp"
#include <Geode/Geode.hpp>
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>
#include <alphalaneous.object_popup_api/include/ObjectNames.hpp>
#include "IconsMaterialDesignIcons.h"

#include "ObjectCategories.hpp"

using namespace geode::prelude;

float buttonSizeValue = 30.f;
const int maxRecentCount = 21;

std::unordered_map<int, CCSprite*> objectSpriteCache;
std::unordered_map<int, std::pair<std::string, CCSprite*>> customObjectSpriteCache;


std::vector<ErGui::ObjectConfig> getCustomObjectsConfig() {
	std::vector<int> ids;
	if (auto dict = GameManager::get()->m_customObjectDict) {
		for (auto* key : CCArrayExt<CCString*>(dict->allKeys())) {
			if (int id = std::atoi(key->getCString())) {
				ids.push_back(id);
			}
		}
	}
	return {{0, ids}};
}


CCSprite* ErGui::getObjectSprite(int id) {
	if (id > 0) {
		auto it = objectSpriteCache.find(id);
		if (it != objectSpriteCache.end()) {
			return it->second;
		}
		auto spr = ErGui::getGameObjectAsSingleSpriteById(id);
		spr->retain();
		objectSpriteCache[id] = spr;
		return spr;
	} else {
		
		auto it = customObjectSpriteCache.find(id);
		if (it != customObjectSpriteCache.end()) {
			return it->second.second; // sprite
		}

		auto objStr = GameManager::get()->stringForCustomObject(id);
		if (objStr.empty()) {
			objStr = "1,914,2,735,3,120,155,1,31,bm8gaWRlYQ==;1,914,2,735,3,90,155,1,31,d2hlcmU=;1,914,2,735,3,60,155,1,31,aXQgaXM=;1,914,2,735,3,150,155,1,31,aSBoYXZl;";
		}
		auto spr = ErGui::getGameObjectsAsSingleSprite(objStr);
		spr->retain();
		customObjectSpriteCache[id] = {objStr, spr};
		return spr;
	}
}


void ErGui::addRecentObjectToList(int objId) {
	if (ErGui::objectCfg[RECENT_TAB_KEY].size() == 0) {
		ErGui::objectCfg[RECENT_TAB_KEY].push_back({0, {}});
	}
	auto &vec = ErGui::objectCfg[RECENT_TAB_KEY][0].objectIdVector;
	const auto it = std::find(vec.begin(), vec.end(), objId);
	if (it != vec.end()) {
		vec.erase(it);
	}
	int next = objId;
	for (int i = 0; i < vec.size(); i++) {
		std::swap(vec[i], next);
	}
	if (vec.size() < maxRecentCount) vec.push_back(next);
}


void ErGui::clearObjectListCache() {
	for (auto [_, v] : objectSpriteCache) {
		v->release();
	}
	objectSpriteCache.clear();

	for (auto [_, v] : customObjectSpriteCache) {
		v.second->release();
	}
	customObjectSpriteCache.clear();
}


void ErGui::reloadCustomObjects() {
	auto ids = getCustomObjectsConfig();
	auto gm = GameManager::get();
	std::unordered_map<std::string, CCSprite*> strMappings;
	std::unordered_map<int, std::pair<std::string, CCSprite*>> newConfig;

	for (auto [_, v] : customObjectSpriteCache) {
		strMappings.insert({v.first, v.second});
	}

	for (int id : ids[0].objectIdVector) {
		auto str = std::string(gm->stringForCustomObject(id));
		auto it = strMappings.find(str);
		if (it != strMappings.end()) {
			newConfig.insert({id, {str, it->second}});
			it->second->retain();
		}
	}

	for (auto [_, v] : customObjectSpriteCache) {
		v.second->release();
	}
	customObjectSpriteCache.clear();

	customObjectSpriteCache = std::move(newConfig);
}


bool ImageButtonFromFrameName(ErGui::ObjectConfig const &objCfg, int j, const char* str_id, ImVec2 imageSize = ImVec2(30.f, 30.f), bool isFavourite = false) {
	bool ret = false;
	int objId = objCfg.objectIdVector[j];
	EditorUI* editorUI = EditorUI::get();

	//Color for selected object
	bool shouldPopStyle = false;
	if (editorUI->m_selectedObjectIndex == objId) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		ImGui::PushStyleColor(ImGuiCol_Border, { 1.f, 1.f, 0.5f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_Button, {
			ImGui::GetStyleColorVec4(ImGuiCol_Button).x + 0.3f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).y + 0.3f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).z + 0.3f,
			ImGui::GetStyleColorVec4(ImGuiCol_Button).w + 0.3f });
		shouldPopStyle = true;
	}

	ImTextureID texture = (ImTextureID)(intptr_t)ErGui::getObjectSprite(objId)->getTexture()->getName();

	if (ImGui::ImageButton(str_id, texture, imageSize, ImVec2(0,1), ImVec2(1,0))) {
		ret = true;
		editorUI->m_selectedObjectIndex = (editorUI->m_selectedObjectIndex != objId) ? objId : 0;
		editorUI->updateGridNodeSize();

	}
	
	//Pop color for selected object
	if (shouldPopStyle) {
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	if (ImGui::BeginPopupContextItem()) {
		ImGui::Text("Object Settings");
		auto FAVOURITES_NAME = "Favourites";
		if (!isFavourite) {
			if (ImGui::Button("Favourite")) {
				auto &cfg = ErGui::objectCfg[FAVOURITES_NAME];
				if (cfg.empty()) cfg.push_back({0, {}});

				if (!ErGui::foundInVector<int>(cfg[0].objectIdVector, objId))
					cfg[0].objectIdVector.push_back(objId);
			}
		} else {
			if (ImGui::Button("Remove")) {
				std::erase(ErGui::objectCfg[FAVOURITES_NAME].at(0).objectIdVector, objId);
			}
		}
		//if (ImGui::Button("Set As Thumbnail")) { 
		//	//This is a new obj confing, so it won't work. Should get address of original config
		//	auto* pointerObjCfg = &objCfg;
		//	pointerObjCfg->thumbnailObjectId = objId;
		//	//ImGui::CloseCurrentPopup();
		//}
		ImGui::EndPopup();
	}

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		std::string newFrameName = ObjectNames::get()->nameForID(objId);
		ImGui::SetTooltip(newFrameName.c_str());
	}

	return ret;
}

void ImageFolderButton(std::vector<ErGui::ObjectConfig> const &visibleButtons, int i, ImVec2 buttonSize = ImVec2(30.f, 30.f)) {

	int folderId = visibleButtons[i].thumbnailObjectId;
	// Если для этой папки ещё нет записи, то по умолчанию она закрыта

	if (folderId == 0) return ;

	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive));
	
	ImTextureID texture = (ImTextureID)(intptr_t)ErGui::getObjectSprite(folderId)->getTexture()->getName();

	std::string folderStrId = "##FOLDER-" + std::to_string(folderId);

	if (ImGui::ImageButton(folderStrId.c_str(), texture, buttonSize, ImVec2(0,1), ImVec2(1,0))) {
		std::string popupStr = "##POPUP-" + std::to_string(folderId);
		ImGui::OpenPopup(popupStr.c_str());
	}

	ImGui::PopStyleColor();

	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float lastButtonX2 = ImGui::GetItemRectMax().x;
	float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

	if (i + 1 < visibleButtons.size() && nextButtonX2 < windowVisibleX2)
		ImGui::SameLine();

}

// this is called foreach object tab
void objectTabCreate(std::string name, std::vector<ErGui::ObjectConfig> const &mySet, ImGuiTextFilter filter, 
				ImVec2 buttonSize, bool isCustomTab = false, bool isFavoriteTab = false) {

	std::vector<ErGui::ObjectConfig> visibleButtons;
	auto editorUI = EditorUI::get();
	auto mod = geode::Mod::get();

	// filter
	if (filter.IsActive()) {
		for (ErGui::ObjectConfig const &oc : mySet) {
			std::vector<int> visibleObjects = {};
			
			for (int i : oc.objectIdVector) {
				auto frameName = ObjectNames::get()->nameForID(i);
				if (filter.PassFilter(frameName.c_str()))
					visibleObjects.push_back(i);
			}
	
			if (!visibleObjects.empty()) {
				// Копируем объект, но заменяем objectIdVector на видимые объекты
				ErGui::ObjectConfig newOc = {oc.thumbnailObjectId, visibleObjects};
				visibleButtons.push_back(newOc);
			}
		}
	} else {
		visibleButtons = mySet;
	}

	if (visibleButtons.empty() && !isCustomTab) return;
	if (visibleButtons.size() == 1 && visibleButtons[0].objectIdVector.empty()) return;

	// draw buttons
	if (ImGui::CollapsingHeader(name.c_str())) {
		float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		
		for (int i = 0; i < visibleButtons.size(); i++) {
			ImageFolderButton(visibleButtons, i, buttonSize);

			std::string popupStr = "##POPUP-" + std::to_string(visibleButtons[i].thumbnailObjectId);
			auto const &objects = visibleButtons[i].objectIdVector;

			if (visibleButtons[i].thumbnailObjectId != 0) {
				if (ImGui::BeginPopup(popupStr.c_str())) {
					for (int j = 0; j < objects.size(); j++) {
						std::string strId = "##OBJECT-" + name + std::to_string(objects[j]);

						if (ImageButtonFromFrameName(visibleButtons[i], j, strId.c_str(), buttonSize, isFavoriteTab)) {
							if (mod->getSavedValue<bool>("autoswitch-to-build-mode", true)) {
								editorUI->m_selectedMode = 2;
							}

							if (mod->getSavedValue<bool>("hide-object-list-popup", true)) {
								ImGui::CloseCurrentPopup();
							}
						}

						if (j + 1 < objects.size() && (j + 1) % 6 != 0)
							ImGui::SameLine();
					}
					ImGui::EndPopup();
				}
			}
			else {
				for (int j = 0; j < objects.size(); j++) {
					std::string strId = "##OBJECT-" + name + std::to_string(objects[j]);
					if (ImageButtonFromFrameName(visibleButtons[i], j, strId.c_str(), buttonSize, isFavoriteTab)) {
						if (mod->getSavedValue<bool>("autoswitch-to-build-mode", true)) {
							editorUI->m_selectedMode = 2;
						}
					}

					float lastButtonX2 = ImGui::GetItemRectMax().x;
					float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

					if ((j + 1 < objects.size() || i + 1 < visibleButtons.size()) && nextButtonX2 < windowVisibleX2)
						ImGui::SameLine();
				}
			}
		}
		ImGui::PopStyleVar();

		if (isCustomTab) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 11));
			ImGui::Separator();
			if (ImGui::Button(ICON_MDI_PLUS, ImVec2(30, 30))) editorUI->onNewCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_MINUS, ImVec2(30, 30))) editorUI->onDeleteCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_ARROW_LEFT_BOLD, ImVec2(30, 30))) editorUI->orderDownCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_ARROW_RIGHT_BOLD, ImVec2(30, 30))) editorUI->orderUpCustomItem(nullptr);
			ImGui::PopStyleVar();
		}
	}
}

void ErGui::renderObjectList() {
	//ErGui::setupMenuColors(ImVec4(0.05f, 0.10f, 0.02f, 0.95f), ImVec4(0.38f, 0.77f, 0.12f, 1.f), ImVec4(0.63f, 0.80f, 0.49f, 1.f));
	if (!ImGui::Begin("Object List")) {
		ImGui::End();
		return;
	}
	
	static ImGuiTextFilter filter;
	filter.Draw();
	
	ImGui::InputFloat("Button Size", &buttonSizeValue);
	ImVec2 buttonSize(buttonSizeValue, buttonSizeValue);

	ErGui::objectCfg[CUSTOM_TAB_KEY] = getCustomObjectsConfig();

	for (const auto& key : keyOrder) {
		objectTabCreate(key, ErGui::objectCfg[key], filter, buttonSize, key == CUSTOM_TAB_KEY, key == FAVOURITES_TAB_KEY);
	}

	ImGui::End();
}


//Settings!
//Close popup after choosing an object - todo
//Buttons Size - Done
//Favourite objects
//Set Object as Thumbnail button
//Recent???
//...
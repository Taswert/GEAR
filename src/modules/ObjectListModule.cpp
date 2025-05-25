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
const std::string FAVOURITES_NAME = "Favourites";

std::pair<const char*, std::vector<ErGui::ObjectConfig>> recentObjects = {
	"Most Recent", {
		{0, {}}
	}
};

std::unordered_map<int, CCSprite*> objectSpriteCache; // todo: все еще не очищается
std::unordered_map<std::string, CCSprite*> customObjectSpriteCache;


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


CCSprite* getObjectSprite(int id) {
	if (id > 0) {
		auto it = objectSpriteCache.find(id);
		if (it != objectSpriteCache.end()) {
			return it->second;
		}
		auto spr = ErGui::getGameObjectAsSingleSpriteById(id);
		spr->retain();
		objectSpriteCache[id] = spr;
		spr->setFlipY(true); // fix for imgui coordinates
		return spr;
	} else {
		auto objStr = GameManager::get()->stringForCustomObject(id);
		if (objStr.empty()) {
			objStr = "1,914,2,735,3,120,155,1,31,bm8gaWRlYQ==;1,914,2,735,3,90,155,1,31,d2hlcmU=;1,914,2,735,3,60,155,1,31,aXQgaXM=;1,914,2,735,3,150,155,1,31,aSBoYXZl;";
		}
		auto it = customObjectSpriteCache.find(objStr);
		if (it != customObjectSpriteCache.end()) {
			return it->second;
		}
		auto spr = ErGui::getGameObjectsAsSingleSprite(objStr);
		spr->retain();
		customObjectSpriteCache[objStr] = spr;
		spr->setFlipY(true); // fix for imgui coordinates
		return spr;
	}
}


void ErGui::addRecentObjectToList(int objId) {
	auto &vec = recentObjects.second.at(0).objectIdVector;
	const auto it = std::find(vec.begin(), vec.end(), objId);
	if (it != vec.end()) {
		vec.erase(it);
	}
	int next = objId;
	for (int i = 0; i < vec.size(); i++) {
		std::swap(vec[i], next);
	}
	if (vec.size() < maxRecentCount) vec.push_back(next);
	// мда, с листом было бы удобней...
}


void ErGui::clearObjectListCache() {
	for (auto [_, v] : objectSpriteCache) {
		v->release();
	}
	objectSpriteCache.clear();

	for (auto [_, v] : customObjectSpriteCache) {
		v->release();
	}
	customObjectSpriteCache.clear();
}

bool ImageButtonFromFrameName(ErGui::ObjectConfig& objCfg, int j, const char* str_id, ImVec2 imageSize = ImVec2(30.f, 30.f), bool isFavourite = false) {
	int objId = objCfg.objectIdVector[j];
	std::string newFrameName = ObjectNames::get()->nameForID(objId);

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

	if (ImGui::Button(str_id, imageSize)) {
		if (editorUI->m_selectedObjectIndex != objId)
			editorUI->m_selectedObjectIndex = objId;
		else
			editorUI->m_selectedObjectIndex = 0;
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
		if (!isFavourite) {
			if (ImGui::Button("Favourite")) {
				if (!ErGui::foundInVector<std::string>(ErGui::keyOrder, FAVOURITES_NAME))
					ErGui::keyOrder.push_back(FAVOURITES_NAME);
				
				if (ErGui::objectCfg[FAVOURITES_NAME].empty()) {
					auto objCfg = ErGui::ObjectConfig(0, std::vector<int>());
					std::vector<ErGui::ObjectConfig> newCfgVec;
					newCfgVec.push_back(objCfg);
					ErGui::objectCfg[FAVOURITES_NAME] = newCfgVec;
				}
				if (!ErGui::foundInVector<int>(ErGui::objectCfg[FAVOURITES_NAME].at(0).objectIdVector, objId))
					ErGui::objectCfg[FAVOURITES_NAME].at(0).objectIdVector.push_back(objId);
			}
		}
		else {
			if (ImGui::Button("Remove")) {
				std::erase(ErGui::objectCfg[FAVOURITES_NAME].at(0).objectIdVector, objId);
				if (ErGui::objectCfg[FAVOURITES_NAME].at(0).objectIdVector.empty())
					std::erase(ErGui::keyOrder, FAVOURITES_NAME);
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

	ErGui::drawSpriteInImGui(getObjectSprite(objId));

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip(newFrameName.c_str());
	}

	return true;
}

void ImageFolderButton(std::vector<ErGui::ObjectConfig> visibleButtons, int i, ImVec2 buttonSize = ImVec2(30.f, 30.f)) {

	int folderId = visibleButtons[i].thumbnailObjectId;
	// Если для этой папки ещё нет записи, то по умолчанию она закрыта

	if (folderId == 0) return ;

	ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive));

	std::string folderStrId = "##FOLDER-";
	folderStrId.append(std::to_string(folderId));
	if (ImGui::Button(folderStrId.c_str(), buttonSize)) {
		std::string popupStr = "##POPUP-";
		popupStr.append(std::to_string(folderId));
		ImGui::OpenPopup(popupStr.c_str());
	}

	ImGui::PopStyleColor();

	ErGui::drawSpriteInImGui(getObjectSprite(folderId));

	float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	float lastButtonX2 = ImGui::GetItemRectMax().x;
	float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

	if (i + 1 < visibleButtons.size() && nextButtonX2 < windowVisibleX2)
		ImGui::SameLine();

}

// this is called foreach object tab
void objectTabCreate(std::string name, std::vector<ErGui::ObjectConfig>& mySet, ImGuiTextFilter filter, ImVec2 buttonSize, bool isCustomTab = false) {

	std::vector<ErGui::ObjectConfig> visibleButtons;

	// filter buttons from mySet
	for (ErGui::ObjectConfig oc : mySet) {
		std::vector<int> visibleObjects = { };
		
		for (int i : oc.objectIdVector) {
			auto frameName = ObjectNames::get()->nameForID(i);
			if (filter.PassFilter(frameName.c_str()))
				visibleObjects.push_back(i);
		}

		if (!visibleObjects.empty()) {
			// Копируем объект, но заменяем objectIdVector на видимые объекты
			ErGui::ObjectConfig newOc = oc;
			newOc.objectIdVector = visibleObjects;
			visibleButtons.push_back(newOc);
		}
	}

	if (visibleButtons.empty() && !isCustomTab) return;

	if (ImGui::CollapsingHeader(name.c_str())) {
		float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		for (int i = 0; i < visibleButtons.size(); i++) {
			ImageFolderButton(visibleButtons, i, buttonSize);
			std::string popupStr = "##POPUP-";
			popupStr.append(std::to_string(visibleButtons[i].thumbnailObjectId));

			if (visibleButtons[i].thumbnailObjectId != 0 && ImGui::BeginPopup(popupStr.c_str())) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i], j, strId.c_str(), buttonSize, FAVOURITES_NAME == name);

					if (j + 1 < visibleButtons[i].objectIdVector.size() && (j + 1) % 6 != 0)
						ImGui::SameLine();
				}
				ImGui::EndPopup();
			}
			else if (visibleButtons[i].thumbnailObjectId == 0) {
				for (int j = 0; j < visibleButtons[i].objectIdVector.size(); j++) {
					std::string strId = std::string("##OBJECT-") + name + std::to_string(visibleButtons[i].objectIdVector[j]);
					ImageButtonFromFrameName(visibleButtons[i], j, strId.c_str(), buttonSize, FAVOURITES_NAME == name);

					float lastButtonX2 = ImGui::GetItemRectMax().x;
					float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;

					if (j + 1 < visibleButtons[i].objectIdVector.size() && nextButtonX2 < windowVisibleX2)
						ImGui::SameLine();
				}
			}
		}
		if (isCustomTab) {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 11));
			ImGui::Separator();
			if (ImGui::Button(ICON_MDI_PLUS, ImVec2(30, 30))) EditorUI::get()->onNewCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_MINUS, ImVec2(30, 30))) EditorUI::get()->onDeleteCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_ARROW_LEFT_BOLD, ImVec2(30, 30))) reinterpret_cast<void(__stdcall*)(EditorUI*, CCObject*)>(geode::base::get() + 0xe3050)(EditorUI::get(), nullptr); //EditorUI::get()->orderDownCustomItem(nullptr);
			ImGui::SameLine();

			if (ImGui::Button(ICON_MDI_ARROW_RIGHT_BOLD, ImVec2(30, 30))) reinterpret_cast<void(__stdcall*)(EditorUI*, CCObject*)>(geode::base::get() + 0xe2fc0)(EditorUI::get(), nullptr); //EditorUI::get()->orderUpCustomItem(nullptr);
			ImGui::PopStyleVar();
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

	for (auto key : keyOrder) {
		objectTabCreate(key, ErGui::objectCfg[key], filter, buttonSize);
	}

	objectTabCreate(recentObjects.first, recentObjects.second, filter, buttonSize);
	auto customConfig = getCustomObjectsConfig();
	objectTabCreate("Custom Objects", customConfig, filter, buttonSize, true);

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
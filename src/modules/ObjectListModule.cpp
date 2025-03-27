#pragma once
#include "ObjectListModule.hpp"
#include <Geode/Geode.hpp>
#include <Geode/GeneratedPredeclare.hpp>
#include <cocos2d.h>

#include "ObjectCategories.hpp"

using namespace geode::prelude;

bool ImageButtonFromFrameName(int objId, ImGuiTextFilter filter, const char* str_id, ImVec2 imageSize = ImVec2(30.f, 30.f), ImVec4 bgCol = ImVec4(0, 0, 0, 0), ImVec4 tintCol = ImVec4(1, 1, 1, 1)) {

	std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(objId);
	if (newFrameName.empty()) return false;
	CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
	if (!frame) return false;


	CCSprite* tempSprite = CCSprite::createWithSpriteFrame(frame);
	auto quad = tempSprite->getQuad();

	float u_min = std::min({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
	float u_max = std::max({ quad.bl.texCoords.u, quad.br.texCoords.u, quad.tl.texCoords.u, quad.tr.texCoords.u });
	float v_min = std::min({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });
	float v_max = std::max({ quad.bl.texCoords.v, quad.br.texCoords.v, quad.tl.texCoords.v, quad.tr.texCoords.v });

	auto texture = tempSprite->getTexture();
	GLuint textureID = texture->getName();
	ImTextureID imguiTexture = (ImTextureID)(intptr_t)textureID;


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
	

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
		ImGui::SetTooltip(newFrameName.c_str());
	}

	//Pop color for selected object
	if (shouldPopStyle) ImGui::PopStyleColor();

	return true;
}

void objectTabCreate(const char* name, std::vector<int> mySet, ImGuiTextFilter filter) {

	std::vector<int> visibleButtons;
	for (int i : mySet) {
		auto frameName = ObjectToolbox::sharedState()->intKeyToFrame(i);
		if (filter.PassFilter(frameName)) {
			visibleButtons.push_back(i);
		}
	}

	if (visibleButtons.empty()) return;

	if (ImGui::CollapsingHeader(name)) {
		float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		
		ImVec2 buttonSize(30.0f, 30.0f);
		//int objCount = 0;
		for (int i = 0; i < visibleButtons.size(); i++) {
			std::string strId = "##";
			strId.append(std::to_string(visibleButtons[i]));
			ImageButtonFromFrameName(visibleButtons[i], filter, strId.c_str(), buttonSize);
			//objCount++;

			float lastButtonX2 = ImGui::GetItemRectMax().x;
			float nextButtonX2 = lastButtonX2 + ImGui::GetStyle().ItemSpacing.x + buttonSize.x;
			
			if (i+1 < visibleButtons.size() && nextButtonX2 < windowVisibleX2)
				ImGui::SameLine();
		}
	}
}

void ErGui::renderObjectList() {
	ImGui::Begin("ObjectList-Module");
	static ImGuiTextFilter filter;
	filter.Draw();

	for (auto object : ErGui::getObjectTabMap()) {
		objectTabCreate(object.first, object.second, filter);
	}
	
	ImGui::End();
}
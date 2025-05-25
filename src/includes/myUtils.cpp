#include "myUtils.hpp"

//gd::string LevelEditorLayer::getSFXIDs() {
//	gd::string str;
//	reinterpret_cast<void(__fastcall*)(LevelEditorLayer*, gd::string*)>(geode::base::get() + 0x2cec40)(this, &str);
//	return str;
//}

namespace ErGui {
	bool isPointInPolygon(const cocos2d::CCPoint& pt, const std::vector<cocos2d::CCPoint>& polygon) {
		bool inside = false;
		int count = static_cast<int>(polygon.size());
		for (int i = 0, j = count - 1; i < count; j = i++)
		{
			const cocos2d::CCPoint& pi = polygon[i];
			const cocos2d::CCPoint& pj = polygon[j];

			// ѕровер€ем пересечение луча, идущего вправо от pt, со стороной многоугольника
			if (((pi.y > pt.y) != (pj.y > pt.y)) &&
				(pt.x < (pj.x - pi.x) * (pt.y - pi.y) / (pj.y - pi.y) + pi.x))
			{
				inside = !inside;
			}
		}
		return inside;
	}

	void setMaxMin(int& value, int max, int min) {
		if (value > max) value = max;
		else if (value < min) value = min;
	}

	void setMin(int& value, int min) {
		if (value < min) value = min;
	}

	float deltaInputFloat(const char* label, float step) {
		ImGui::PushID(label);

		float result = 0;
		
		ImGui::Text(label);
		ImGui::SameLine(70.f);
		if (ImGui::Button("-")) {
			result = step * -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("+")) {
			result = step;
		}

		ImGui::PopID();
		return result;
	}

	int deltaInputIntImproved(const char* label, int max, int min, int step) {
		ImGui::PushID(label);

		int result = 0;
		if (ImGui::Button("-##el1Minus")) {
			result = step * -1;
		}
		std::string textStr = label;
		if (max != min)
			textStr += " (" + std::to_string(min) + ".." + std::to_string(max) + ")";
		else 
			textStr += " (" + std::to_string(min) + ")";
		ImGui::SameLine();
		ImGui::Text(textStr.c_str());
		ImGui::SameLine();
		if (ImGui::Button("+##el1Plus")) {
			result = step;
		}

		ImGui::PopID();
		return result;
	}
	void drawSpriteInImGui(CCSprite* tempSprite) {
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

	void drawFrameInImGui(CCSpriteFrame* frame) {
		CCSprite* tempSprite = CCSprite::createWithSpriteFrame(frame);
		drawSpriteInImGui(tempSprite);
	}
	
	void initImGuiStyling() {
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();

		style.WindowPadding = ImVec2(8.f, 8.f);
		style.WindowRounding = 4.f;
		style.WindowBorderSize = 0.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 4.f;
		style.ChildBorderSize = 1.f;
		style.PopupRounding = 4.f;
		style.PopupBorderSize = 1.f;
		style.FramePadding = ImVec2(6.f, 3.f);
		style.FrameRounding = 2.f;
		style.FrameBorderSize = 0.f;
		style.ItemSpacing = ImVec2(10.f, 4.f);
		style.ItemInnerSpacing = ImVec2(4.f, 4.f);
		style.CellPadding = ImVec2(4.f, 2.f);
		style.TouchExtraPadding = ImVec2(0.f, 0.f);
		style.IndentSpacing = 24.f;
		style.ScrollbarSize = 14.f;
		style.ScrollbarRounding = 6.f;
		style.GrabMinSize = 12.f;
		style.GrabRounding = 4.f;
		style.LogSliderDeadzone = 4.f;
		style.TabRounding = 4.f;
		style.TabBorderSize = 0.f;
		style.TabBarBorderSize = 2.f;
		style.TabBarOverlineSize = 0.f;
		style.TableAngledHeadersAngle = 35.f;
		style.TableAngledHeadersTextAlign = ImVec2(0.5f, 0.f);
		style.ColorButtonPosition = ImGuiDir_Left;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.5f, 0.65f);
		style.SeparatorTextBorderSize = 4.f;
		style.SeparatorTextAlign = ImVec2(0.f, 0.f);
		style.SeparatorTextPadding = ImVec2(40.f, 2.f);
		style.DockingSeparatorSize = 4.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.79f, 0.79f, 0.79f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.71f, 0.28f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.84f, 0.52f, 0.04f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.84f, 0.56f, 0.14f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.71f, 0.28f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.78f, 0.78f, 0.78f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.75f, 0.75f, 0.75f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.54f, 0.07f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.50f, 0.08f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.65f, 0.11f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.65f, 0.11f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 0.65f, 0.11f, 0.95f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.97f, 0.68f, 0.25f, 0.80f);
		colors[ImGuiCol_Tab] = ImVec4(0.53f, 0.53f, 0.53f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.82f, 0.54f, 0.11f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(1.00f, 0.65f, 0.11f, 0.00f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.20f, 0.20f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(1.00f, 0.65f, 0.11f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextLink] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.65f, 0.11f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavCursor] = ImVec4(1.00f, 0.65f, 0.11f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void enableClicks() {
		//if (ImGui::IsItemHovered() || ImGui::IsWindowHovered()) {
		//	ImGuiCocos::get().setShouldPassClicks(false);
		//}
	}

	void addObjectToUndoList(GameObject* obj, UndoCommand command) {
		GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, command));
		GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
	}

	void addObjectsToUndoList(CCArray* objArr, UndoCommand command) {
		CCArray* copyObjArr = CCArray::create();
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			auto objCopy = GameObjectCopy::create(obj);
			copyObjArr->addObject(objCopy);
			//objCopy->resetObject();
		}
		GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(copyObjArr, command));
		GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
	}
}
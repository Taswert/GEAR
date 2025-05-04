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

			// Проверяем пересечение луча, идущего вправо от pt, со стороной многоугольника
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
		if (ImGui::Button("-")) {
			result = step * -1;
		}
		ImGui::SameLine();
		ImGui::Text(label);
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

	void drawImageInImGui(CCSpriteFrame* frame) {
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

	void setupMenuColors(ImVec4 BGColor, ImVec4 HoveredColor, ImVec4 TextFieldColor) {
		ImVec4* colors = ImGui::GetStyle().Colors;

		// Основные элементы UI
		//colors[ImGuiCol_WindowBg] =				BGColor;		//ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		//colors[ImGuiCol_ChildBg] =				BGColor;		//ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		//colors[ImGuiCol_Border] =				ImVec4(0.50f, 0.50f, 0.50f, 0.50f);

		// Рамки и заголовки
		//colors[ImGuiCol_FrameBg] =				BGColor; //ImVec4(0.50f, 0.50f, 0.50f, 0.50f);	//ImVec4(0.20f, 0.21f, 0.23f, 1.00f);
		//colors[ImGuiCol_TitleBgActive] =		BGColor; //ImVec4(0.50f, 0.50f, 0.50f, 1.00f);	//ImVec4(0.16f, 0.29f, 0.48f, 1.00f);

		//ImGui::PushStyleColor(ImGuiCol_FrameBg,			BGColor);
		//ImGui::PushStyleColor(ImGuiCol_TitleBgActive,	BGColor);

		// Меню и вкладки
		//colors[ImGuiCol_Button] =				ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f);	//ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
		//colors[ImGuiCol_ButtonHovered] =		ImVec4(HoveredColor.x - 0.00f, HoveredColor.y - 0.00f, HoveredColor.z - 0.00f, HoveredColor.w - 0.00f);	//ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
		//colors[ImGuiCol_ButtonActive] =			ImVec4(HoveredColor.x - 0.15f, HoveredColor.y - 0.15f, HoveredColor.z - 0.15f, HoveredColor.w - 0.15f);	//ImVec4(0.15f, 0.20f, 0.25f, 1.00f);

		//colors[ImGuiCol_Header] =				ImVec4(HoveredColor.x - 0.00f, HoveredColor.y - 0.00f, HoveredColor.z - 0.00f, HoveredColor.w - 0.00f);	//ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
		//colors[ImGuiCol_HeaderHovered] =		ImVec4(HoveredColor.x + 0.10f, HoveredColor.y + 0.10f, HoveredColor.z + 0.10f, HoveredColor.w + 0.10f);	//ImVec4(0.40f, 0.45f, 0.50f, 1.00f);
		//colors[ImGuiCol_HeaderActive] =			ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f);	//ImVec4(0.20f, 0.25f, 0.30f, 1.00f);

		//colors[ImGuiCol_Separator] =			ImVec4(0.30f, 0.30f, 0.30f, 0.50f);	ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		//colors[ImGuiCol_SeparatorHovered] =		ImVec4(0.30f, 0.30f, 0.30f, 0.50f);	ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
		//colors[ImGuiCol_SeparatorActive] =		ImVec4(0.30f, 0.30f, 0.30f, 0.50f);	ImVec4(0.70f, 0.70f, 0.90f, 1.00f);

		//colors[ImGuiCol_Tab] =					ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f);	//ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
		//colors[ImGuiCol_TabHovered] =				ImVec4(HoveredColor.x - 0.00f, HoveredColor.y - 0.00f, HoveredColor.z - 0.00f, HoveredColor.w - 0.00f);	//ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
		//colors[ImGuiCol_TabActive] =				ImVec4(HoveredColor.x - 0.05f, HoveredColor.y - 0.05f, HoveredColor.z - 0.05f, HoveredColor.w - 0.05f);	//ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
		//colors[ImGuiCol_TabUnfocused] =			ImVec4(HoveredColor.x - 0.15f, HoveredColor.y - 0.15f, HoveredColor.z - 0.15f, HoveredColor.w - 0.15f);	//ImVec4(0.15f, 0.20f, 0.25f, 1.00f);
		//colors[ImGuiCol_TabUnfocusedActive] =		ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f);	//ImVec4(0.20f, 0.25f, 0.30f, 1.00f);

		ImGui::PushStyleColor(ImGuiCol_Tab,					ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f));
		ImGui::PushStyleColor(ImGuiCol_TabHovered,			ImVec4(HoveredColor.x - 0.00f, HoveredColor.y - 0.00f, HoveredColor.z - 0.00f, HoveredColor.w - 0.00f));
		ImGui::PushStyleColor(ImGuiCol_TabActive,			ImVec4(HoveredColor.x - 0.05f, HoveredColor.y - 0.05f, HoveredColor.z - 0.05f, HoveredColor.w - 0.05f));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused,		ImVec4(HoveredColor.x - 0.15f, HoveredColor.y - 0.15f, HoveredColor.z - 0.15f, HoveredColor.w - 0.15f));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive,	ImVec4(HoveredColor.x - 0.10f, HoveredColor.y - 0.10f, HoveredColor.z - 0.10f, HoveredColor.w - 0.10f));


		// Текстовое поле и подсказки
		//colors[ImGuiCol_TextSelectedBg] =		ImVec4(TextFieldColor.x, TextFieldColor.y, TextFieldColor.z, 0.35f); //ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		//colors[ImGuiCol_NavHighlight] =			ImVec4(TextFieldColor.x, TextFieldColor.y, TextFieldColor.z, 1.00f); //ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	}

	void enableClicks() {
		//if (ImGui::IsItemHovered() || ImGui::IsWindowHovered()) {
		//	ImGuiCocos::get().setShouldPassClicks(false);
		//}
	}

	
}
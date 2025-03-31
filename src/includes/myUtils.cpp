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

	void createNewKeyframeAnim(EditorUI* self) {
		return reinterpret_cast<void(__fastcall*)(EditorUI*)>(geode::base::get() + 0x1179f0)(self);
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

	void enableClicks() {
		//if (ImGui::IsItemHovered() || ImGui::IsWindowHovered()) {
		//	ImGuiCocos::get().setShouldPassClicks(false);
		//}
	}

	
}
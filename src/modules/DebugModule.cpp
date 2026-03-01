#include "DebugModule.hpp"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "PropertiesModule.hpp"
#include "../classes/GearEditorUI.hpp"
#include "ContextMenuModule.hpp"
#include <imgui.h>

void ErGui::renderDebugModule() {
	if (g_showDebugModule) {
		auto gEui = GearEditorUI::get();
		ImGui::Begin("Debug");
		ImGui::Checkbox("EditorUI", &gEui->m_bVisible);
		//ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
		ImGui::Text("ErGui::showProperties: %d", ErGui::showProperties);
		ImGui::Text("Is Zooming: %d", gEui->m_fields->m_isSmoothZooming);
		ImGui::Text("Zoom Scale: %.3f", gEui->m_editorLayer->m_objectLayer->getScale());
		ImGui::Text("Selected Object address: %p", gEui->m_selectedObject);
		ImGui::Text("Pointer to member m_positionX: %p", &GameObject::m_positionX);
		ImGui::Text("F2 is %s", ImGui::IsKeyDown(ImGuiKey::ImGuiKey_F2) ? "Pressed" : "Not Pressed"); // why are you not working ughh
		ImGui::Text(fmt::format("m_wasShifting: {}", gEui->m_fields->m_wasShifting).c_str());
		ImGui::Text("Object under cursor: %p", ErGui::objectUnderCursor);
		ImGui::Text("Last Under Cursor index: %d", gEui->m_fields->m_lastUnderCursorIndex);

		ImGui::Text("--TS--");
		GJTransformControl* tc = gEui->m_transformControl;
		auto ts = gEui->m_transformState;
		ImGui::Text(fmt::format("m_transformSkewX: {}, {}", ts.m_transformSkewX.x, ts.m_transformSkewX.y).c_str());
		ImGui::Text(fmt::format("m_transformSkewY: {}, {}", ts.m_transformSkewY.x, ts.m_transformSkewY.y).c_str());
		ImGui::Text(fmt::format("m_transformPosition: {}, {}", ts.m_transformPosition.x, ts.m_transformPosition.y).c_str());
		ImGui::Text(fmt::format("m_transformRotation: {}", ts.m_transformRotation).c_str());
		ImGui::Text(fmt::format("m_angleX/Y: {}, {}", ts.m_angleX, ts.m_angleY).c_str());
		ImGui::Text(fmt::format("m_transformScaleX/Y: {}, {}", ts.m_transformScaleX, ts.m_transformScaleY).c_str());

		ImGui::Text("--TC--");
		int i = 0;
		for (auto warpButton : CCArrayExt<CCSprite*>(tc->m_warpSprites)) {
			ImGui::Text(fmt::format("warpButton {}: {} / {}", i, warpButton->getPositionX(), warpButton->getPositionY()).c_str());
			i++;
		}
		ImGui::End();
	}
}
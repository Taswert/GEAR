#include "DebugModule.hpp"
#include "PropertiesModule.hpp"
#include "../classes/GearEditorUI.hpp"
#include <imgui.h>

void ErGui::renderDebugModule() {
	if (g_showDebugModule) {
		ImGui::Begin("Debug");
		ImGui::Checkbox("EditorUI", &EditorUI::get()->m_bVisible);
		//ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
		ImGui::Text("ErGui::showProperties: %d", ErGui::showProperties);
		ImGui::Text("Is Zooming: %d", GearEditorUI::get()->m_fields->m_isSmoothZooming);
		ImGui::Text("Zoom Scale: %.3f", EditorUI::get()->m_editorLayer->m_objectLayer->getScale());
		ImGui::Text("Selected Object address: %p", EditorUI::get()->m_selectedObject);
		ImGui::Text("Pointer to member m_positionX: %p", &GameObject::m_positionX);
		ImGui::Text("F2 is %s", ImGui::IsKeyDown(ImGuiKey::ImGuiKey_F2) ? "Pressed" : "Not Pressed"); // why are you not working ughh
		ImGui::End();
	}
}
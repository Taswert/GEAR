#include "DebugModule.hpp"
#include "PropertiesModule.hpp"
#include "../classes/GearEditorUI.hpp"

void ErGui::renderDebugModule() {
	if (g_showDebugModule) {
		ImGui::Begin("Debug");
		ImGui::Checkbox("EditorUI", &EditorUI::get()->m_bVisible);
		//ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
		ImGui::Text("ErGui::showProperties: %d", ErGui::showProperties);
		ImGui::Text("Is Zooming: %d", GearEditorUI::get()->m_fields->m_isSmoothZooming);
		ImGui::Text("Zoom Scale: %.3f", EditorUI::get()->m_editorLayer->m_objectLayer->getScale());
		ImGui::Text("Selected Object address: %p", EditorUI::get()->m_selectedObject);
		ImGui::End();
	}
}
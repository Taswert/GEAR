#include "DebugModule.hpp"
#include "PropertiesModule.hpp"

void ErGui::renderDebugModule() {
	if (g_showDebugModule) {
		ImGui::Begin("Goida");
		ImGui::Checkbox("EditorUI", &EditorUI::get()->m_bVisible);
		//ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
		ImGui::Text("ErGui::showProperties: %d", ErGui::showProperties);
		ImGui::End();
	}
}
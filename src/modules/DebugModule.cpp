#include "DebugModule.hpp"
#include "PropertiesModule.hpp"

void ErGui::renderDebugModule() {
	if (g_showDebugModule) {
		ImGui::Begin("Debug");
		//ImGui::Text("g_selectionVersion: % d", ErGui::g_selectVersion);
		ImGui::Text("ErGui::showProperties: %d", ErGui::showProperties);
		ImGui::End();
	}
}
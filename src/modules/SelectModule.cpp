#include "SelectModule.hpp"

namespace ErGui {
	void renderSelectModule() {
		ImGui::Begin("Select-Module");

		if (ImGui::Checkbox("Lasso", &isLassoEnabled)) {
			if (isLassoEnabled) {
				//GameManager::sharedState()->setGameVariable("0003", true);
				lassoPatch->enable();
			}
			else {
				//GameManager::sharedState()->setGameVariable("0003", false);
				lassoPatch->disable();
			}
		}
		
		if (ImGui::Button("lel")) {
			GameManager::sharedState()->getEditorLayer()->m_editorUI->moveGamelayer({200.f, 0.f});
		}
		ImGui::End();
	}
}
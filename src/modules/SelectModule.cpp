#include "SelectModule.hpp"

namespace ErGui {
	void renderSelectModule() {
		ImGui::Begin("Select-Module");
		ErGui::enableClicks();


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
			auto m1 = GameManager::sharedState()->getEditorLayer()->m_unknownE58;

			std::cout << "M1: " << m1.size() << "\n\n";
		}
		ImGui::End();
	}
}
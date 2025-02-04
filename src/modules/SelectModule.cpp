#include "SelectModule.hpp"

namespace ErGui {
	void renderSelectModule() {
		ImGui::Begin("Select-Module");

		if (ImGui::Checkbox("Lasso", &isLassoEnabled)) {
			if (isLassoEnabled) {
				lassoPatch->enable();
			}
			else {
				lassoPatch->disable();
			}
		}

		ImGui::End();
	}
}
#pragma once
#include "CameraSettings.hpp"

namespace ErGui {
	void renderCameraSettings() {
		ImGui::Begin("CameraSettings-Module");
		ErGui::enableClicks();


		ImGui::SetNextItemWidth(120.f);
		ImGui::InputFloat("Camera2-X", &cam2_offsetX);
		ImGui::SetNextItemWidth(120.f);
		ImGui::InputFloat("Camera2-Y", &cam2_offsetY);

		ImGui::Checkbox("Camera2-PREV", &cam2_prevMode);
		ImGui::Checkbox("Camera2-ANIM", &cam2_prevAnim);
		ImGui::Checkbox("Camera2-PART", &cam2_prevPart);
		ImGui::Checkbox("Camera2-SHAD", &cam2_prevShad);

		
		ImGui::End();
	}
}
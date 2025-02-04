#include "TransformObjectModule.hpp"
using namespace ErGui;

void skewFuncX(GameObject* obj, float step) {
	//if (obj->getRotationX() + step)
	float alpha = obj->getRotationX() - obj->getRotationY();
	float h = std::cos(alpha * (3.14159f / 180.f)) * obj->m_scaleY;

	alpha = std::fmod(alpha + step, 360.f);
	float newRot = obj->getRotationX() + step;
	obj->setRotationX(newRot);

	float modScaleY = std::pow(std::cos(alpha * (3.14159f / 180.f)), -1) * h;
	int minusMod = (obj->m_scaleY > 0 && modScaleY < 0) || (obj->m_scaleY < 0 && modScaleY > 0) ? -1 : 1;

	obj->setScaleY(modScaleY * minusMod);
	obj->m_scaleY = modScaleY * minusMod;
}

void skewFuncY(GameObject* obj, float step) {
	float alpha = obj->getRotationY();
	float h = std::cos(alpha * (3.14159f / 180.f)) * obj->m_scaleX;

	alpha += step;
	obj->setRotationY(alpha);

	float modScaleX = std::pow(std::cos(alpha * (3.14159f / 180.f)), -1) * h;
	int minusMod = (obj->m_scaleX > 0 && modScaleX < 0) || (obj->m_scaleX < 0 && modScaleX > 0) ? -1 : 1;

	obj->setScaleX(modScaleX * minusMod);
	obj->m_scaleX = modScaleX * minusMod;
}

void renderForObject(GameObject* obj) {
	//Also, make legacy options for every tab
	if (ImGui::CollapsingHeader("-----| Move |-----")) {
		float posX = obj->getRealPosition().x;
		float posY = obj->getRealPosition().y;

		float oldPosX = posX;
		float oldPosY = posY;

		ImGui::InputFloat("MoveStep", &moveStep, 1.f);

		if (ImGui::Button("0.5##MoveStepPreset")) moveStep = 0.5f;	ImGui::SameLine();
		if (ImGui::Button("2##MoveStepPreset")) moveStep = 2.f;		ImGui::SameLine();
		if (ImGui::Button("7.5##MoveStepPreset")) moveStep = 7.5f;	ImGui::SameLine();
		if (ImGui::Button("10##MoveStepPreset")) moveStep = 10.f;	ImGui::SameLine();
		if (ImGui::Button("15##MoveStepPreset")) moveStep = 15.f;	ImGui::SameLine();
		if (ImGui::Button("30##MoveStepPreset")) moveStep = 30.f;	ImGui::SameLine();
		if (ImGui::Button("150##MoveStepPreset")) moveStep = 150.f;	ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::InputFloat("PosX", &posX, moveStep);
		ImGui::InputFloat("PosY", &posY, moveStep);
		if (oldPosX != posX || oldPosY != posY)
			GameManager::sharedState()->getEditorLayer()->m_editorUI->moveObject(obj, { posX - oldPosX, posY - oldPosY });
	}


	if (ImGui::CollapsingHeader("-----| Rotation |-----")) {
		float rotX = obj->getRotationX();
		float rotY = obj->getRotationY();
		ImGui::InputFloat("RotStep", &rotationStep, 1.f);
		if (ImGui::Button("1##RotateStepPreset")) rotationStep = 1.f;		ImGui::SameLine();
		if (ImGui::Button("10##RotateStepPreset")) rotationStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##RotateStepPreset")) rotationStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##RotateStepPreset")) rotationStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##RotateStepPreset")) rotationStep = 45.f;		ImGui::SameLine();
		if (ImGui::Button("90##RotateStepPreset")) rotationStep = 90.f;		ImGui::SameLine();
		if (ImGui::Button("180##RotateStepPreset")) rotationStep = 180.f;	ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::InputFloat("RotX", &rotX, rotationStep);
		ImGui::InputFloat("RotY", &rotY, rotationStep);
		float rotDelta = ErGui::deltaInputFloat("Rotate", rotationStep);
		//ImGui::InputFloat("RotDelta", &rotDelta, rotationStep); //make custom InputFloat with rotation display
		ImGui::Text("Rot: %.2f", obj->getRotation());

		rotX = std::fmod(rotX + rotDelta, 360);
		rotY = std::fmod(rotY + rotDelta, 360);

		obj->setRotationX(rotX);
		obj->setRotationY(rotY);

	}

	if (ImGui::CollapsingHeader("-----| Scale |-----")) {
		float scaleX = obj->m_scaleX;
		float scaleY = obj->m_scaleY;

		ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
		if (ImGui::Button("0.01##ScaleStepPreset")) scaleStep = 0.01f;		ImGui::SameLine();
		if (ImGui::Button("0.05##ScaleStepPreset")) scaleStep = 0.05f;		ImGui::SameLine();
		if (ImGui::Button("0.1##ScaleStepPreset")) scaleStep = 0.1f;		ImGui::SameLine();
		if (ImGui::Button("0.25##ScaleStepPreset")) scaleStep = 0.25f;		ImGui::SameLine();
		if (ImGui::Button("0.5##ScaleStepPreset")) scaleStep = 0.5f;		ImGui::SameLine();
		if (ImGui::Button("1##ScaleStepPreset")) scaleStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("2##ScaleStepPreset")) scaleStep = 2.f;			ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::InputFloat("ScaleX", &scaleX, scaleStep);
		ImGui::InputFloat("ScaleY", &scaleY, scaleStep);
		float scaleDelta = ErGui::deltaInputFloat("Scale", scaleStep);
		//ImGui::InputFloat("ScaleDelta", &scaleDelta, scaleStep); 
		ImGui::Text("Scale: %.2f", obj->m_scaleX);

		//GameManager::sharedState()->getEditorLayer()->m_editorUI
		short flipX = obj->isFlipX() ? -1 : 1;
		short flipY = obj->isFlipY() ? -1 : 1;
		obj->setScaleX((scaleX + scaleDelta) * flipX);
		obj->setScaleY((scaleY + scaleDelta) * flipY);
		obj->m_scaleX = scaleX + scaleDelta;
		obj->m_scaleY = scaleY + scaleDelta;
	}

	if (ImGui::CollapsingHeader("-----| Skew |-----")) {
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset")) skewStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset")) skewStep = 5.f;			ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset")) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset")) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset")) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset")) skewStep = 45.f;		ImGui::SameLine();
		ImGui::Text("Presets");

		if (float skewXDelta = ErGui::deltaInputFloat("SkewX", skewStep)) {
			auto uo = GameManager::sharedState()->getEditorLayer()->m_editorUI->createUndoObject(UndoCommand::Transform, 1);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(uo);
			skewFuncX(obj, skewXDelta);
		}
		ImGui::SameLine();
		if (float skewYDelta = ErGui::deltaInputFloat("SkewY", skewStep)) {
			auto uo = GameManager::sharedState()->getEditorLayer()->m_editorUI->createUndoObject(UndoCommand::Transform, 1);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(uo);
			skewFuncY(obj, skewYDelta);
		}

	}

	ImGui::Text("-----| Circle Tool |-----"); //check mat's mod literally 
}

void renderForArray(CCArray* objArr) {
	//Also, make legacy options for every tab
	auto lel = GameManager::sharedState()->getEditorLayer();
	if (ImGui::CollapsingHeader("-----| Move |-----")) {
		ImGui::InputFloat("MoveStep", &moveStep, 1.f);
		if (ImGui::Button("0.5##MoveStepPreset")) moveStep = 0.5f;	ImGui::SameLine();
		if (ImGui::Button("2##MoveStepPreset")) moveStep = 2.f;		ImGui::SameLine();
		if (ImGui::Button("7.5##MoveStepPreset")) moveStep = 7.5f;	ImGui::SameLine();
		if (ImGui::Button("10##MoveStepPreset")) moveStep = 10.f;	ImGui::SameLine();
		if (ImGui::Button("15##MoveStepPreset")) moveStep = 15.f;	ImGui::SameLine();
		if (ImGui::Button("30##MoveStepPreset")) moveStep = 30.f;	ImGui::SameLine();
		if (ImGui::Button("150##MoveStepPreset")) moveStep = 150.f;	ImGui::SameLine();
		ImGui::Text("Presets");

		float posXDelta = ErGui::deltaInputFloat("PosX", moveStep);
		ImGui::SameLine();
		float posYDelta = ErGui::deltaInputFloat("PosY", moveStep);
		if (posXDelta || posYDelta) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				GameManager::sharedState()->getEditorLayer()->m_editorUI->moveObject(obj, { posXDelta, posYDelta });
			}
		}
	}


	if (ImGui::CollapsingHeader("-----| Rotation |-----")) {
		ImGui::InputFloat("RotStep", &rotationStep, 1.f);
		if (ImGui::Button("1##RotateStepPreset")) rotationStep = 1.f;		ImGui::SameLine();
		if (ImGui::Button("10##RotateStepPreset")) rotationStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##RotateStepPreset")) rotationStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##RotateStepPreset")) rotationStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##RotateStepPreset")) rotationStep = 45.f;		ImGui::SameLine();
		if (ImGui::Button("90##RotateStepPreset")) rotationStep = 90.f;		ImGui::SameLine();
		if (ImGui::Button("180##RotateStepPreset")) rotationStep = 180.f;	ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::Checkbox("Position Snap##RotPosSnap", &rotateObjectsPositionSnap);
		
		float rotDelta = ErGui::deltaInputFloat("Rotate", rotationStep);
		ImGui::SameLine();
		float rotDeltaX = ErGui::deltaInputFloat("RotateX", rotationStep);
		ImGui::SameLine();
		float rotDeltaY = ErGui::deltaInputFloat("RotateY", rotationStep);
		
		//ImGui::InputFloat("RotX", &rotX, rotationStep);
		//ImGui::InputFloat("RotY", &rotY, rotationStep);
		//ImGui::InputFloat("RotDelta", &rotDelta, rotationStep); //make custom InputFloat with rotation display
		//ImGui::Text("Rot: %.2f", obj->getRotation());
		

		if (rotDeltaX || rotDeltaY || rotDelta) {
			if (!rotateObjectsPositionSnap)
				lel->m_editorUI->rotateObjects(objArr, rotDelta, { 0, 0 });
			else {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					float rotX = obj->getRotationX();
					float rotY = obj->getRotationY();

					rotX = std::fmod(rotX + rotDeltaX + rotDelta, 360);
					rotY = std::fmod(rotY + rotDeltaY + rotDelta, 360);

					obj->setRotationX(rotX);
					obj->setRotationY(rotY);
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("-----| Scale |-----")) { //it's not saving when exit the level for some reason...
		ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
		if (ImGui::Button("0.01##ScaleStepPreset")) scaleStep = 0.01f;		ImGui::SameLine();
		if (ImGui::Button("0.05##ScaleStepPreset")) scaleStep = 0.05f;		ImGui::SameLine();
		if (ImGui::Button("0.1##ScaleStepPreset")) scaleStep = 0.1f;		ImGui::SameLine();
		if (ImGui::Button("0.25##ScaleStepPreset")) scaleStep = 0.25f;		ImGui::SameLine();
		if (ImGui::Button("0.5##ScaleStepPreset")) scaleStep = 0.5f;		ImGui::SameLine();
		if (ImGui::Button("1##ScaleStepPreset")) scaleStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("2##ScaleStepPreset")) scaleStep = 2.f;			ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::Checkbox("Position Snap##ScalePosSnap", &scaleObjectsPositionSnap);

		float scaleDelta = ErGui::deltaInputFloat("Scale", scaleStep);
		ImGui::SameLine();
		float scaleDeltaX = ErGui::deltaInputFloat("ScaleX", scaleStep);
		ImGui::SameLine();
		float scaleDeltaY = ErGui::deltaInputFloat("ScaleY", scaleStep);

		//ImGui::InputFloat("ScaleX", &scaleX, scaleStep);
		//ImGui::InputFloat("ScaleY", &scaleY, scaleStep);
		//ImGui::InputFloat("ScaleDelta", &scaleDelta, scaleStep); //make custom InputFloat with rotation display
		//ImGui::Text("Scale: %.2f", obj->m_scaleX);
	
		if (scaleDeltaX || scaleDeltaY || scaleDelta) {
			if (!scaleObjectsPositionSnap) //To rewrite
				lel->m_editorUI->scaleObjects(objArr, scaleDelta + scaleDeltaX, scaleDelta + scaleDeltaY, { 0, 0 }, ObjectScaleType::XY, false);
			else {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					float scaleX = obj->m_scaleX;
					float scaleY = obj->m_scaleY;

					short flipX = obj->isFlipX() ? -1 : 1;
					short flipY = obj->isFlipY() ? -1 : 1;

					obj->setScaleX((scaleX + scaleDelta + scaleDeltaX) * flipX);
					obj->setScaleY((scaleY + scaleDelta + scaleDeltaY) * flipY);
					obj->m_scaleX = scaleX + scaleDelta + scaleDeltaX;
					obj->m_scaleY = scaleY + scaleDelta + scaleDeltaY;
				}
			}
		}

		
	}

	if (ImGui::CollapsingHeader("-----| Skew |-----")) {
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset")) skewStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset")) skewStep = 5.f;			ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset")) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset")) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset")) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset")) skewStep = 45.f;		ImGui::SameLine();
		ImGui::Text("Presets");

		ImGui::Checkbox("Position Snap##SkewPosSnap", &skewObjectsPositionSnap);
		float skewXDelta = ErGui::deltaInputFloat("SkewX", skewStep);
		ImGui::SameLine();
		float skewYDelta = ErGui::deltaInputFloat("SkewY", skewStep);

		if (skewXDelta || skewYDelta) {
			auto uo = GameManager::sharedState()->getEditorLayer()->m_editorUI->createUndoObject(UndoCommand::Transform, 1);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(uo);
			if (!skewObjectsPositionSnap) {
				std::cout << "So uh, this is wip ig...\n";
			}
			else {
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					skewFuncX(obj, skewXDelta);
					skewFuncY(obj, skewYDelta);
				}
			}
		}
	}
}

void ErGui::renderTransformation() {
	ImGui::Begin("Transformation-Module");
	auto obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject;
	auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;
	if (obj) {
		renderForObject(obj);
	}
	else if (objArr->count() > 0) {
		renderForArray(objArr);
	} 
		

	ImGui::End();
}
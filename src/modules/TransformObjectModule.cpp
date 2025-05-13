#include "TransformObjectModule.hpp"
using namespace ErGui;

void skewFuncX(GameObject* obj, float step) {
	//if (obj->getRotationX() + step)
	float alpha = obj->getRotationX() - obj->getRotationY();
	float h = std::cos(alpha * (3.14159f / 180.f)) * obj->m_scaleY;

	alpha = std::fmod(alpha + step, 360.f);
	float newRot = obj->getRotationX() + step;

	float modScaleY = std::pow(std::cos(alpha * (3.14159f / 180.f)), -1) * h;
	int minusMod = (obj->m_scaleY > 0 && modScaleY < 0) || (obj->m_scaleY < 0 && modScaleY > 0) ? -1 : 1;
	if ((static_cast<int>(newRot) % 90 != 0 || newRot == obj->getRotationY()) && minusMod != -1) {
		obj->setRotationX(newRot);

		obj->setScaleY(modScaleY * minusMod);
		obj->m_scaleY = modScaleY * minusMod;
	}
}

void skewFuncY(GameObject* obj, float step) {
	float alpha = obj->getRotationY() - obj->getRotationX();
	float h = std::cos(alpha * (3.14159f / 180.f)) * obj->m_scaleX;
	
	alpha = std::fmod(alpha + step, 360.f);
	float newRot = obj->getRotationY() + step;

	float modScaleX = std::pow(std::cos(alpha * (3.14159f / 180.f)), -1) * h;
	int minusMod = (obj->m_scaleX > 0 && modScaleX < 0) || (obj->m_scaleX < 0 && modScaleX > 0) ? -1 : 1;
	if ((static_cast<int>(newRot) % 90 != 0 || newRot == obj->getRotationX()) && minusMod != -1) {
		obj->setRotationY(newRot);

		obj->setScaleX(modScaleX * minusMod);
		obj->m_scaleX = modScaleX * minusMod;
	}
}

//Mat's code. Thank you Mat.
float calc(float angle) {
	return -sinf(angle / 180.f * 3.141592f - 3.141592f / 2.f) * angleSquish;
}

void renderCircleTool() {
	if (ImGui::CollapsingHeader("-----| Circle Tool |-----")) {
		if (ImGui::DragFloat("Arc", &angleArc)) {
			if (angleArc < 0.f) angleArc = 0.f;
		}
		if (ImGui::DragFloat("Step", &angleStep)) {
			if (angleStep < 0.f) angleStep = 0.f;
		}
		ImGui::DragFloat("Squish", &angleSquish);

		auto lel = GameManager::sharedState()->getEditorLayer();
		auto editorUI = lel->m_editorUI;

		const auto amt = static_cast<size_t>(std::ceilf(angleArc / angleStep) - 1.f);
		const auto objCount = amt * editorUI->getSelectedObjects()->count();

		ImGui::Text("Objects: %d", objCount);
		ImGui::Text("Copies: %d", amt);

		if (ImGui::Button("Apply") && angleArc > 0.f && angleStep > 0.f) {

			if (editorUI->m_selectedObjects) {
				auto* objs = CCArray::create();
				float off_acc = calc(0);
				for (float i = 1; i * angleStep < angleArc; ++i) {
					editorUI->onDuplicate(nullptr);
					auto selected = editorUI->getSelectedObjects();
					editorUI->rotateObjects(selected, angleStep, { 0.f, 0.f });

					const float angle = i * angleStep;

					if (angleSquish != 0.f) {
						float off_y = calc(angle) - off_acc;

						for (auto obj : CCArrayExt<GameObject*>(selected)) {
							editorUI->moveObject(obj, { 0, off_y });
						}

						off_acc = calc(angle);
					}

					// remove undo object for pasting the objs
					lel->m_undoObjects->removeLastObject();
					objs->addObjectsFromArray(selected);
				}
				lel->m_undoObjects->addObject(UndoObject::createWithArray(objs, UndoCommand::Paste));
				// second argument is ignoreSelectFilter
				editorUI->selectObjects(objs, true);
			}
		}
	}
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
		if (oldPosX != posX || oldPosY != posY) {
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			GameManager::sharedState()->getEditorLayer()->m_editorUI->moveObject(obj, { posX - oldPosX, posY - oldPosY });
		}
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

		if (ImGui::InputFloat("RotX", &rotX, rotationStep)) {
			rotX = std::fmod(rotX, 360);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
		}
		if (ImGui::InputFloat("RotY", &rotY, rotationStep)) {
			rotY = std::fmod(rotY, 360);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
		}
		float rotDelta = ErGui::deltaInputFloat("Rotate", rotationStep);
		//ImGui::InputFloat("RotDelta", &rotDelta, rotationStep); //make custom InputFloat with rotation display
		ImGui::Text("Rot: %.2f", obj->getRotation());

		if (rotDelta) {
			rotX = std::fmod(rotX + rotDelta, 360);
			rotY = std::fmod(rotY + rotDelta, 360);
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
		}
		
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

		if (ImGui::InputFloat("ScaleX", &scaleX, scaleStep)) {
			short flipX = obj->isFlipX() ? -1 : 1;
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			obj->setScaleX(scaleX * flipX);
			obj->m_scaleX = scaleX;
		}
		if (ImGui::InputFloat("ScaleY", &scaleY, scaleStep)) {
			short flipY = obj->isFlipY() ? -1 : 1;
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			obj->setScaleY(scaleY * flipY);
			obj->m_scaleY = scaleY;
		}
		float scaleDelta = ErGui::deltaInputFloat("Scale", scaleStep);
		//ImGui::InputFloat("ScaleDelta", &scaleDelta, scaleStep); 
		ImGui::Text("Scale: %.2f", obj->m_scaleX);

		if (scaleDelta) {
			short flipX = obj->isFlipX() ? -1 : 1;
			short flipY = obj->isFlipY() ? -1 : 1;
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			obj->setScaleX((scaleX + scaleDelta) * flipX);
			obj->setScaleY((scaleY + scaleDelta) * flipY);
			obj->m_scaleX = scaleX + scaleDelta;
			obj->m_scaleY = scaleY + scaleDelta;
		}

		//GameManager::sharedState()->getEditorLayer()->m_editorUI
		
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
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			skewFuncX(obj, skewXDelta);
		}
		ImGui::SameLine();
		if (float skewYDelta = ErGui::deltaInputFloat("SkewY", skewStep)) {
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, UndoCommand::Transform));
			skewFuncY(obj, skewYDelta);
		}

	}

	renderCircleTool();
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
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(objArr, UndoCommand::Transform));
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
				GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(objArr, UndoCommand::Transform));
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
			GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(objArr, UndoCommand::Transform));
			if (!scaleObjectsPositionSnap) {
				//lel->m_editorUI->scaleObjects(objArr, scaleDelta + scaleDeltaX, scaleDelta + scaleDeltaY, { 0, 0 }, ObjectScaleType::XY, false);
				CCPoint groupCenter = lel->m_editorUI->getGroupCenter(objArr, false);
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					CCPoint offset = { (obj->getPosition().x - groupCenter.x) / obj->getScaleX(), (obj->getPosition().y - groupCenter.y) / obj->getScaleY() };
					//std::cout << obj->getPosition().y					<< " " << groupCenter.y << " " << obj->getScaleY()					<< "\n";
					//std::cout << obj->getPosition().y - groupCenter.y	<< " " << (obj->getPosition().x - groupCenter.y) / obj->getScaleY()	<< "\n";

					float scaleX = obj->m_scaleX;
					float scaleY = obj->m_scaleY;

					short flipX = obj->isFlipX() ? -1 : 1;
					short flipY = obj->isFlipY() ? -1 : 1;

					obj->setScaleX((scaleX + scaleDelta + scaleDeltaX) * flipX);
					obj->setScaleY((scaleY + scaleDelta + scaleDeltaY) * flipY);
					obj->m_scaleX = scaleX + scaleDelta + scaleDeltaX;
					obj->m_scaleY = scaleY + scaleDelta + scaleDeltaY;

					//std::cout << obj->getScaleY() << " " << offset.y * obj->getScaleY() << " " << offset.y * obj->getScaleY() + groupCenter.y << "\n\n";

					if (scaleDelta)
						obj->setPosition({ offset.x * obj->getScaleX() + groupCenter.x , offset.y * obj->getScaleY() + groupCenter.y });
					else if (scaleDeltaX)
						obj->setPosition({ offset.x * obj->getScaleX() + groupCenter.x , obj->getPosition().y });
					else if (scaleDeltaY)
						obj->setPosition({ obj->getPosition().x , offset.y * obj->getScaleY() + groupCenter.y });
				}
			}
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
			//GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(objArr, UndoCommand::Transform));
			if (!skewObjectsPositionSnap) {
				std::cout << "So uh, this is wip ig...\n";
			}
			else {
				GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(objArr, UndoCommand::Transform));
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					skewFuncX(obj, skewXDelta);
					skewFuncY(obj, skewYDelta);
				}
			}
		}
	}

	renderCircleTool();
}

void ErGui::renderTransformation() {
	//ErGui::setupMenuColors(ImVec4(0.02f, 0.10f, 0.11f, 0.95f), ImVec4(0.09f, 0.82f, 0.85f, 1.f), ImVec4(0.12f, 0.88f, 0.91f, 1.f));
	ImGui::Begin("Transformation-Module");
	ErGui::enableClicks();


	auto obj = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObject;
	auto objArr = GameManager::sharedState()->getEditorLayer()->m_editorUI->m_selectedObjects;
	if (obj) {
		renderForObject(obj);
	}
	else if (objArr->count() > 0) {
		renderForArray(objArr);
	}
	else {
		ImGui::Text("Object is not selected...");
	}
	
	//for (int i = 0; i < 5; i++) ImGui::PopStyleColor();

	ImGui::End();
}
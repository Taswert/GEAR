#include "TransformObjectModule.hpp"
using namespace ErGui;


void addObjectToUndoList(GameObject* obj, UndoCommand command) {
	GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::create(obj, command));
	GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
}

void addObjectsToUndoList(CCArray* objArr, UndoCommand command) {
	CCArray* copyObjArr = CCArray::create();
	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		auto objCopy = GameObjectCopy::create(obj);
		copyObjArr->addObject(objCopy);
		//objCopy->resetObject();
	}
	GameManager::sharedState()->getEditorLayer()->m_undoObjects->addObject(UndoObject::createWithArray(copyObjArr, command));
	GameManager::sharedState()->getEditorLayer()->m_redoObjects->removeAllObjects();
}

float deg2rad(float d) {
	return d * (3.14159265f / 180.f);
}

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

// Групповая версия:
void skewFuncXGroup(CCArray* objArr, float step) {
	if (objArr == nullptr || objArr->count() == 0) return;

	float baseY = GameManager::sharedState()->m_levelEditorLayer->m_editorUI->getGroupCenter(objArr, false).y;

	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		float oldX = obj->getPositionX();
		float oldRot = obj->getRotationX();

		skewFuncX(obj, step);

		float newRot = obj->getRotationX();
		if (newRot == oldRot) continue;

		float dy = obj->getPositionY() - baseY;

		float origX = oldX - std::tan(deg2rad(oldRot)) * dy;
		float newX = origX + std::tan(deg2rad(newRot)) * dy;

		obj->setPositionX(newX);
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

void skewFuncYGroup(CCArray* objArr, float step) {
	if (objArr == nullptr || objArr->count() == 0) return;

	float baseX = GameManager::sharedState()->m_levelEditorLayer->m_editorUI->getGroupCenter(objArr, false).x;

	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		float oldY = obj->getPositionY();
		float oldRot = obj->getRotationY();

		skewFuncY(obj, step);

		float newRot = obj->getRotationY();
		if (newRot == oldRot) continue;

		float dx = obj->getPositionX() - baseX;

		float origY = oldY + std::tan(deg2rad(oldRot)) * dx;
		float newY = origY - std::tan(deg2rad(newRot)) * dx;

		obj->setPositionY(newY);
	}
}


//Mat's code. Thank you Mat.
float calc(float angle) {
	return -sinf(angle / 180.f * 3.141592f - 3.141592f / 2.f) * angleSquish;
}

void renderCircleTool() {
	if (ImGui::CollapsingHeader("-----| Circle Tool |-----")) {
		ImGui::Text("Arc/Step");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth((ErGui::INPUT_ITEM_WIDTH - 10.f) / 2.f);
		if (ImGui::DragFloat("##Arc", &angleArc, 1.f, 0.f, 0.f, "%.2f")) {
			if (angleArc < 0.f) angleArc = 0.f;
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth((ErGui::INPUT_ITEM_WIDTH - 10.f)/ 2.f);
		if (ImGui::DragFloat("##Step", &angleStep, 1.f, 0.f, 0.f, "%.2f")) {
			if (angleStep < 0.f) angleStep = 0.f;
		}

		ImGui::Text("Squish");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH);
		ImGui::DragFloat("##Squish", &angleSquish, 1.f, 0.f, 0.f, "%.2f");

		auto lel = GameManager::sharedState()->getEditorLayer();
		auto editorUI = lel->m_editorUI;

		const auto amt = static_cast<size_t>(std::ceilf(angleArc / angleStep) - 1.f);
		const auto objCount = amt * editorUI->getSelectedObjects()->count();

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
		ImGui::SameLine();
		ImGui::Text("Objects: %d", objCount);
		ImGui::SameLine();
		ImGui::Text("Copies: %d", amt);
	}
}

void renderForObject(GameObject* obj) {

	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;


	//if (ImGui::CollapsingHeader("-----| Move |-----")) {
		float posX = obj->getRealPosition().x;
		float posY = obj->getRealPosition().y;

		float oldPosX = posX;
		float oldPosY = posY;

		if (ImGui::BeginPopup("MoveStepPopup")) {
			ImGui::InputFloat("MoveStep", &moveStep, 1.f);

			if (ImGui::Button("0.5##MoveStepPreset")) moveStep = 0.5f;	ImGui::SameLine();
			if (ImGui::Button("2##MoveStepPreset")) moveStep = 2.f;		ImGui::SameLine();
			if (ImGui::Button("7.5##MoveStepPreset")) moveStep = 7.5f;	ImGui::SameLine();
			if (ImGui::Button("10##MoveStepPreset")) moveStep = 10.f;	ImGui::SameLine();
			if (ImGui::Button("15##MoveStepPreset")) moveStep = 15.f;	ImGui::SameLine();
			if (ImGui::Button("30##MoveStepPreset")) moveStep = 30.f;	ImGui::SameLine();
			if (ImGui::Button("150##MoveStepPreset")) moveStep = 150.f;	ImGui::SameLine();
			ImGui::Text("Presets");

			ImGui::EndPopup();
		}
			

		ImGui::Text("Position");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputFloat("##PosX", &posX, moveStep, moveStep * 5, "%.2f");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		ImGui::InputFloat("##PosY", &posY, moveStep, moveStep * 5, "%.2f");
		ImGui::SameLine();
		if (ImGui::Button("S##Position-Presets")) {
			ImGui::OpenPopup("MoveStepPopup");
		}

		float dPosX = posX - oldPosX;
		float dPosY = posY - oldPosY;

		if (dPosX || dPosY) {
			addObjectToUndoList(obj, UndoCommand::Transform);
			editorUI->m_rotationControl->setPosition({ editorUI->m_rotationControl->getPositionX() + dPosX, editorUI->m_rotationControl->getPositionY() + dPosY });
			editorUI->moveObject(obj, { dPosX, dPosY });
		}
	//}


	//if (ImGui::CollapsingHeader("-----| Rotation |-----")) {
		if (ImGui::BeginPopup("RotationStepPopup")) {
			ImGui::InputFloat("RotStep", &rotationStep, 1.f);
			if (ImGui::Button("1##RotateStepPreset")) rotationStep = 1.f;		ImGui::SameLine();
			if (ImGui::Button("10##RotateStepPreset")) rotationStep = 10.f;		ImGui::SameLine();
			if (ImGui::Button("15##RotateStepPreset")) rotationStep = 15.f;		ImGui::SameLine();
			if (ImGui::Button("30##RotateStepPreset")) rotationStep = 30.f;		ImGui::SameLine();
			if (ImGui::Button("45##RotateStepPreset")) rotationStep = 45.f;		ImGui::SameLine();
			if (ImGui::Button("90##RotateStepPreset")) rotationStep = 90.f;		ImGui::SameLine();
			if (ImGui::Button("180##RotateStepPreset")) rotationStep = 180.f;	ImGui::SameLine();
			ImGui::Text("Presets");

			ImGui::EndPopup();
		}
		
		float rotX = obj->getRotationX();
		float rotY = obj->getRotationY();

		float oldRotX = rotX;
		float oldRotY = rotY;

		ImGui::Text("Rotation");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		if (ImGui::InputFloat("##RotX", &rotX, rotationStep, rotationStep * 5, "%.2f")) {
			int mod = 1;
			if (oldRotX > rotX) mod = -1;

			rotX = std::fmod(rotX, 360);
			addObjectToUndoList(obj, UndoCommand::Transform);
			obj->setRotationX(rotX);
			
			if (rotLock) {
				rotY += rotationStep * mod;
				rotY = std::fmod(rotY, 360);
				obj->setRotationY(rotY);
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		if (ImGui::InputFloat("##RotY", &rotY, rotationStep, rotationStep * 5, "%.2f")) {
			int mod = 1;
			if (oldRotY > rotY) mod = -1;

			rotY = std::fmod(rotY, 360);
			addObjectToUndoList(obj, UndoCommand::Transform);
			obj->setRotationY(rotY);

			if (rotLock) {
				rotX += rotationStep * mod;
				rotX = std::fmod(rotX, 360);
				obj->setRotationX(rotX);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("S##Rotation-Presets")) {
			ImGui::OpenPopup("RotationStepPopup");
		}
		ImGui::SameLine();
		ImGui::Checkbox("##Rotation-Lock", &rotLock);

	//}

	//if (ImGui::CollapsingHeader("-----| Scale |-----")) {
		if (ImGui::BeginPopup("ScaleStepPopup")) {
			ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
			if (ImGui::Button("0.01##ScaleStepPreset")) scaleStep = 0.01f;		ImGui::SameLine();
			if (ImGui::Button("0.05##ScaleStepPreset")) scaleStep = 0.05f;		ImGui::SameLine();
			if (ImGui::Button("0.1##ScaleStepPreset")) scaleStep = 0.1f;		ImGui::SameLine();
			if (ImGui::Button("0.25##ScaleStepPreset")) scaleStep = 0.25f;		ImGui::SameLine();
			if (ImGui::Button("0.5##ScaleStepPreset")) scaleStep = 0.5f;		ImGui::SameLine();
			if (ImGui::Button("1##ScaleStepPreset")) scaleStep = 1.f;			ImGui::SameLine();
			if (ImGui::Button("2##ScaleStepPreset")) scaleStep = 2.f;			ImGui::SameLine();
			ImGui::Text("Presets");

			ImGui::EndPopup();
		}
		
		float scaleX = obj->m_scaleX;
		float scaleY = obj->m_scaleY;

		float oldScaleX = scaleX;
		float oldScaleY = scaleY;

		ImGui::Text("Scale");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		if (ImGui::InputFloat("##ScaleX", &scaleX, scaleStep, scaleStep * 5, "%.2f")) {
			int mod = 1;
			if (oldScaleX > scaleX) mod = -1;

			short flipX = obj->isFlipX() ? -1 : 1;

			addObjectToUndoList(obj, UndoCommand::Transform);
			obj->setScaleX(scaleX * flipX);
			obj->m_scaleX = scaleX;

			if (scaleLock) {
				scaleY += scaleStep * mod;
				short flipY = obj->isFlipY() ? -1 : 1;
				obj->setScaleY(scaleY * flipY);
				obj->m_scaleY = scaleY;
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
		if (ImGui::InputFloat("##ScaleY", &scaleY, scaleStep, scaleStep * 5, "%.2f")) {
			int mod = 1;
			if (oldScaleY > scaleY) mod = -1;

			short flipY = obj->isFlipY() ? -1 : 1;

			addObjectToUndoList(obj, UndoCommand::Transform);
			obj->setScaleY(scaleY * flipY);
			obj->m_scaleY = scaleY;

			if (scaleLock) {
				scaleX += scaleStep * mod;
				short flipX = obj->isFlipX() ? -1 : 1;
				obj->setScaleX(scaleX * flipX);
				obj->m_scaleX = scaleX;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("S##Scale-Presets")) {
			ImGui::OpenPopup("ScaleStepPopup");
		}
		ImGui::SameLine();
		ImGui::Checkbox("##Scale-Lock", &scaleLock);
		//float scaleDelta = ErGui::deltaInputFloat("Scale", scaleStep);
		//ImGui::Text("Scale: %.2f", obj->m_scaleX);

		//if (scaleDelta) {
		//	short flipX = obj->isFlipX() ? -1 : 1;
		//	short flipY = obj->isFlipY() ? -1 : 1;
		//
		//	addObjectToUndoList(obj, UndoCommand::Transform);
		//	obj->setScaleX((scaleX + scaleDelta) * flipX);
		//	obj->setScaleY((scaleY + scaleDelta) * flipY);
		//	obj->m_scaleX = scaleX + scaleDelta;
		//	obj->m_scaleY = scaleY + scaleDelta;
		//}

		//GameManager::sharedState()->getEditorLayer()->m_editorUI
		
	//}

	//if (ImGui::CollapsingHeader("-----| Skew |-----")) {
		//ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		//if (ImGui::Button("1##SkewStepPreset")) skewStep = 1.f;			ImGui::SameLine();
		//if (ImGui::Button("5##SkewStepPreset")) skewStep = 5.f;			ImGui::SameLine();
		//if (ImGui::Button("10##SkewStepPreset")) skewStep = 10.f;		ImGui::SameLine();
		//if (ImGui::Button("15##SkewStepPreset")) skewStep = 15.f;		ImGui::SameLine();
		//if (ImGui::Button("30##SkewStepPreset")) skewStep = 30.f;		ImGui::SameLine();
		//if (ImGui::Button("45##SkewStepPreset")) skewStep = 45.f;		ImGui::SameLine();
		//ImGui::Text("Presets");

		if (float skewXDelta = ErGui::deltaInputFloat("SkewX", skewStep)) {
			addObjectToUndoList(obj, UndoCommand::Transform);
			skewFuncX(obj, skewXDelta);
		}
		ImGui::SameLine();
		ImGui::Button("S##SkewX-Step");
		if (float skewYDelta = ErGui::deltaInputFloat("SkewY", skewStep)) {
			addObjectToUndoList(obj, UndoCommand::Transform);
			skewFuncY(obj, skewYDelta);
		}
		ImGui::SameLine();
		ImGui::Button("S##SkewY-Step");

	//}

	renderCircleTool();
}

void renderForArray(CCArray* objArr) {
	auto gameManager = GameManager::sharedState();
	auto lel = gameManager->getEditorLayer();
	auto editorUI = lel->m_editorUI;
	
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
		float posYDelta = ErGui::deltaInputFloat("PosY", moveStep);
		if (posXDelta || posYDelta) {
			editorUI->m_rotationControl->setPosition({ editorUI->m_rotationControl->getPositionX() + posXDelta, editorUI->m_rotationControl->getPositionY() + posYDelta });
			addObjectsToUndoList(objArr, UndoCommand::Transform);
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				editorUI->moveObject(obj, { posXDelta, posYDelta });
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
		float rotDeltaX = ErGui::deltaInputFloat("RotateX", rotationStep);
		float rotDeltaY = ErGui::deltaInputFloat("RotateY", rotationStep);


		if (rotDeltaX || rotDeltaY || rotDelta) {
			addObjectsToUndoList(objArr, UndoCommand::Transform);
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

	if (ImGui::CollapsingHeader("-----| Scale |-----")) {
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
		float scaleDeltaX = ErGui::deltaInputFloat("ScaleX", scaleStep);
		float scaleDeltaY = ErGui::deltaInputFloat("ScaleY", scaleStep);

	
		if (scaleDeltaX || scaleDeltaY || scaleDelta) {
			addObjectsToUndoList(objArr, UndoCommand::Transform);
			if (!scaleObjectsPositionSnap) {
				CCPoint groupCenter = lel->m_editorUI->getGroupCenter(objArr, false);
				for (auto obj : CCArrayExt<GameObject*>(objArr)) {
					CCPoint offset = { (obj->getPosition().x - groupCenter.x) / obj->getScaleX(), (obj->getPosition().y - groupCenter.y) / obj->getScaleY() };

					float scaleX = obj->m_scaleX;
					float scaleY = obj->m_scaleY;

					short flipX = obj->isFlipX() ? -1 : 1;
					short flipY = obj->isFlipY() ? -1 : 1;

					obj->setScaleX((scaleX + scaleDelta + scaleDeltaX) * flipX);
					obj->setScaleY((scaleY + scaleDelta + scaleDeltaY) * flipY);
					obj->m_scaleX = scaleX + scaleDelta + scaleDeltaX;
					obj->m_scaleY = scaleY + scaleDelta + scaleDeltaY;

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
			addObjectsToUndoList(objArr, UndoCommand::Transform);
			if (!skewObjectsPositionSnap) {
				skewFuncXGroup(objArr, skewXDelta);
				skewFuncYGroup(objArr, skewYDelta);
			}
			else {
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
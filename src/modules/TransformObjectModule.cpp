#include "TransformObjectModule.hpp"
#include "CustomImGuiWidgets.hpp"
using namespace ErGui;
static bool s_resetStaticRotationScale = false;

float deg2rad(float d) {
	return d * (3.14159265f / 180.f);
}

void skewFuncX(GameObject* obj, float step, float* skewX, float* oldSkewX) {
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
	else {
		*skewX = std::fmod(*oldSkewX, 90);
	}
}

void skewFuncXGroup(CCArray* objArr, float step, float* skewX, float* oldSkewX) {
	if (objArr == nullptr || objArr->count() == 0) return;

	float baseY = GameManager::sharedState()->m_levelEditorLayer->m_editorUI->getGroupCenter(objArr, false).y;

	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		float oldX = obj->getPositionX();
		float oldRot = obj->getRotationX();

		skewFuncX(obj, step, skewX, oldSkewX);

		float newRot = obj->getRotationX();
		if (newRot == oldRot) continue;

		float dy = obj->getPositionY() - baseY;

		float origX = oldX - std::tan(deg2rad(oldRot)) * dy;
		float newX = origX + std::tan(deg2rad(newRot)) * dy;

		obj->setPositionX(newX);
	}
}

void skewFuncY(GameObject* obj, float step, float* skewY, float* oldSkewY) {
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
	else {
		*skewY = std::fmod(*oldSkewY, 90);
	}
}

void skewFuncYGroup(CCArray* objArr, float step, float* skewY, float* oldSkewY) {
	if (objArr == nullptr || objArr->count() == 0) return;

	float baseX = GameManager::sharedState()->m_levelEditorLayer->m_editorUI->getGroupCenter(objArr, false).x;

	for (auto obj : CCArrayExt<GameObject*>(objArr)) {
		float oldY = obj->getPositionY();
		float oldRot = obj->getRotationY();

		skewFuncY(obj, step, skewY, oldSkewY);

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
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##PosX", &posX, moveStep, moveStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##PosY", &posY, moveStep, moveStep * 5, "%.2f");
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

	ImGui::Separator();



	// ----- Rotation -----
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
	
	float rot = obj->getRotation();
	float rotX = obj->getRotationX();
	float rotY = obj->getRotationY();

	float oldRot = rot;
	float oldRotX = rotX;
	float oldRotY = rotY;

	ImGui::Text("Rotation");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 6.f / 4.f + ImGui::GetStyle().ItemSpacing.x);
	if (ErGui::BetterDragFloat("##Rot", &rot, rotationStep, rotationStep * 5, "%.2f")) {
		rot = std::fmod(rot, 360);
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setRotationX(std::fmod(rotX + rot - oldRot, 360));
		obj->setRotationY(std::fmod(rotY + rot - oldRot, 360));
	}
	ImGui::SameLine();
	if (ImGui::Button("S##Rotation-Presets")) {
		ImGui::OpenPopup("RotationStepPopup");
	}

	ImGui::Text("X / Y");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	if (ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##RotX", &rotX, rotationStep, rotationStep * 5, "%.2f")) {
		rotX = std::fmod(rotX, 360);
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setRotationX(rotX);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	if (ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##RotY", &rotY, rotationStep, rotationStep * 5, "%.2f")) {
		rotY = std::fmod(rotY, 360);
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setRotationY(rotY);
	}
	
	ImGui::Separator();



	// ----- Scale -----
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
	
	float scale = obj->m_scaleX;
	float scaleX = obj->m_scaleX;
	float scaleY = obj->m_scaleY;

	float oldScale = scale;
	float oldScaleX = scaleX;
	float oldScaleY = scaleY;

	ImGui::Text("Scale");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 6.f / 4.f + ImGui::GetStyle().ItemSpacing.x);
	if (ErGui::BetterDragFloat("##Scale", &scale, scaleStep, scaleStep * 5, "%.2f", 0.01f)) {
		addObjectToUndoList(obj, UndoCommand::Transform);

		float scaleMod = obj->m_scaleY / obj->m_scaleX;
		if (obj->m_scaleX == 0) {
			scaleMod = 1;
			obj->m_scaleX = obj->m_scaleY;
		}

		short flipX = obj->isFlipX() ? -1 : 1;
		obj->m_scaleX += scale - oldScale;
		obj->setScaleX(obj->m_scaleX * flipX);

		short flipY = obj->isFlipY() ? -1 : 1;
		obj->m_scaleY += (scale - oldScale) * scaleMod;
		obj->setScaleY(obj->m_scaleY * flipY);
	}
	ImGui::SameLine();
	if (ImGui::Button("S##Scale-Presets")) {
		ImGui::OpenPopup("ScaleStepPopup");
	}

	ImGui::Text("X / Y");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	if (ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##ScaleX", &scaleX, scaleStep, scaleStep * 5, "%.2f", 0.01f )) {
		short flipX = obj->isFlipX() ? -1 : 1;
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setScaleX(scaleX * flipX);
		obj->m_scaleX = scaleX;
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	if (ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##ScaleY", &scaleY, scaleStep, scaleStep * 5, "%.2f", 0.01f )) {
		short flipY = obj->isFlipY() ? -1 : 1;
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setScaleY(scaleY * flipY);
		obj->m_scaleY = scaleY;
	}
	ImGui::Separator();


	// ----- Skew -----
	if (ImGui::BeginPopup("SkewStepPopup")) {
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset")) skewStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset")) skewStep = 5.f;			ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset")) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset")) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset")) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset")) skewStep = 45.f;		ImGui::SameLine();
		ImGui::Text("Presets");
		ImGui::EndPopup();
	}

	static float skewX = 0.f;
	static float skewY = 0.f;
	if (s_resetStaticRotationScale) {
		skewX = 0.f;
		skewY = 0.f;
		s_resetStaticRotationScale = false;
	}
	float oldSkewX = skewX;
	float oldSkewY = skewY;

	ImGui::Text("Skew");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##SkewX", &skewX, skewStep, skewStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##SkewY", &skewY, skewStep, skewStep * 5, "%.2f");

	ImGui::SameLine();
	if (ImGui::Button("S##Skew-Step"))
		ImGui::OpenPopup("SkewStepPopup");

	float skewXDelta = skewX - oldSkewX;
	float skewYDelta = skewY - oldSkewY;


	if (skewXDelta) {
		addObjectToUndoList(obj, UndoCommand::Transform);
		skewFuncX(obj, skewXDelta, &skewX, &oldSkewX);
	}
	if (skewYDelta) {
		addObjectToUndoList(obj, UndoCommand::Transform);
		skewFuncY(obj, skewYDelta, &skewY, &oldSkewY);
	}


	renderCircleTool();
}

void renderForArray(CCArray* objArr) {
	
	// ----- Setup ----- 
	auto gameManager = GameManager::sharedState();
	auto lel = gameManager->getEditorLayer();
	auto editorUI = lel->m_editorUI;
	
	CCPoint groupCenter = editorUI->getGroupCenter(objArr, false);
	CCPoint oldGroupCenter = groupCenter;

	static float scale = 1.f;
	static float scaleX = 1.f;
	static float scaleY = 1.f;
	static float rotation = 0.f;
	static float rotationX = 0.f;
	static float rotationY = 0.f;
	static float skewX = 0.f;
	static float skewY = 0.f;

	if (s_resetStaticRotationScale) {
		scale = 1.f;
		scaleX = 1.f;
		scaleY = 1.f;
		rotation = 0.f;
		rotationX = 0.f;
		rotationY = 0.f;
		skewX = 0.f;
		skewY = 0.f;
		s_resetStaticRotationScale = false;
	}

	float oldScale = scale;
	float oldScaleX = scaleX;
	float oldScaleY = scaleY;
	float oldRotation = rotation;
	float oldRotationX = rotationX;
	float oldRotationY = rotationY;
	float oldSkewX = skewX;
	float oldSkewY = skewY;


	// ----- Position -----
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
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##PosX", &groupCenter.x, moveStep, moveStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##PosY", &groupCenter.y, moveStep, moveStep * 5, "%.2f");
	ImGui::SameLine();
	if (ImGui::Button("S##Position-Presets")) {
		ImGui::OpenPopup("MoveStepPopup");
	}
	ImGui::Separator();

	float posXDelta = groupCenter.x - oldGroupCenter.x;
	float posYDelta = groupCenter.y - oldGroupCenter.y;
	if (posXDelta || posYDelta) {
		editorUI->m_rotationControl->setPosition({ editorUI->m_rotationControl->getPositionX() + posXDelta, editorUI->m_rotationControl->getPositionY() + posYDelta });
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			editorUI->moveObject(obj, { posXDelta, posYDelta });
		}
	}



	// ----- Rotation -----
	if (ImGui::BeginPopup("RotStepPopup")) {
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

	// -- All Rotation
	ImGui::Text("Rotation");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 6.f / 4.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::BetterDragFloat("##Rotation", &rotation, rotationStep, rotationStep * 5, "%.2f");
	ImGui::SameLine();
	if (ImGui::Button("S##Rotation-Presets")) {
		ImGui::OpenPopup("RotStepPopup");
	}

	// -- XY Rotation
	ImGui::Text("X / Y");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##RotationX", &rotationX, rotationStep, rotationStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##RotationY", &rotationY, rotationStep, rotationStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetItemTooltip("Position Snap");
	ImGui::Checkbox("##RotPosSnap", &rotateObjectsPositionSnap);

	ImGui::Separator();

	// -- Apply Rotation
	float rotDelta = rotation - oldRotation;
	float rotDeltaX = rotationX - oldRotationX;
	float rotDeltaY = rotationY - oldRotationY;
	if (rotDelta || rotDeltaX || rotDeltaY) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		if (!rotateObjectsPositionSnap && rotDelta)
			lel->m_editorUI->rotateObjects(objArr, rotDelta, { 0, 0 });
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				float rotX = obj->getRotationX();
				float rotY = obj->getRotationY();

				rotX = std::fmod(rotX + rotDelta + rotDeltaX, 360);
				rotY = std::fmod(rotY + rotDelta + rotDeltaY, 360);

				obj->setRotationX(rotX);
				obj->setRotationY(rotY);
			}
		}
	}



	// ----- Scale -----
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

	// -- All Scale
	ImGui::Text("Scale");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 6.f / 4.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::BetterDragFloat("##Scale", &scale, scaleStep, scaleStep * 5, "%.2f", 0.01f);
	ImGui::SameLine();
	if (ImGui::Button("S##Scale-Presets")) {
		ImGui::OpenPopup("ScaleStepPopup");
	}

	// -- XY Scale
	ImGui::Text("X / Y");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##ScaleX", &scaleX, scaleStep, scaleStep * 5, "%.2f", 0.01f);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##ScaleY", &scaleY, scaleStep, scaleStep * 5, "%.2f", 0.01f);
	ImGui::SameLine();
	ImGui::SetItemTooltip("Position Snap");
	ImGui::Checkbox("##ScalePosSnap", &scaleObjectsPositionSnap);

	ImGui::Separator();

	float scaleDelta = scale - oldScale;
	float scaleDeltaX = scaleX - oldScaleX;
	float scaleDeltaY = scaleY - oldScaleY;

	
	if (scaleDeltaX || scaleDeltaY || scaleDelta) {
		groupCenter = editorUI->getGroupCenter(objArr, false);
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		if (!scaleObjectsPositionSnap) {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				CCPoint offset = { 
					obj->getScaleX() != 0.f ? (obj->getPosition().x - groupCenter.x) / obj->getScaleX() : (obj->getPosition().x - groupCenter.x),
					obj->getScaleY() != 0.f ? (obj->getPosition().y - groupCenter.y) / obj->getScaleY() : (obj->getPosition().y - groupCenter.y)
				};

				float scaleX = obj->m_scaleX;
				float scaleY = obj->m_scaleY;

				float resultScaleX = scaleX + scaleDelta + scaleDeltaX;
				float resultScaleY = scaleY + scaleDelta + scaleDeltaY;

				short flipX = obj->isFlipX() ? -1 : 1;
				short flipY = obj->isFlipY() ? -1 : 1;

				obj->setScaleX((resultScaleX) * flipX);
				obj->setScaleY((resultScaleY) * flipY);
				obj->m_scaleX = resultScaleX;
				obj->m_scaleY = resultScaleY;

				if (scaleDelta) {
					obj->setPosition({ offset.x * obj->getScaleX() + groupCenter.x , offset.y * obj->getScaleY() + groupCenter.y });
				}
				else if (scaleDeltaX) {
					obj->setPosition({ offset.x * obj->getScaleX() + groupCenter.x , obj->getPosition().y });
				}
				else if (scaleDeltaY) {
					obj->setPosition({ obj->getPosition().x , offset.y * obj->getScaleY() + groupCenter.y });
				}
			}
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				float scaleX = obj->m_scaleX;
				float scaleY = obj->m_scaleY;

				float resultScaleX = scaleX + scaleDelta + scaleDeltaX;
				float resultScaleY = scaleY + scaleDelta + scaleDeltaY;

				short flipX = obj->isFlipX() ? -1 : 1;
				short flipY = obj->isFlipY() ? -1 : 1;

				obj->setScaleX((resultScaleX) * flipX);
				obj->setScaleY((resultScaleY) * flipY);
				obj->m_scaleX = resultScaleX;
				obj->m_scaleY = resultScaleY;
			}
		}
	}


	// ----- Skew -----
	if (ImGui::BeginPopup("SkewStepPopup")) {
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset")) skewStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset")) skewStep = 5.f;			ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset")) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset")) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset")) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset")) skewStep = 45.f;		ImGui::SameLine();
		ImGui::Text("Presets");
		ImGui::EndPopup();
	}

	ImGui::Text("Skew");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(255, 66, 66, 255), "##SkewX", &skewX, skewStep, skewStep * 5, "%.2f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::INPUT_ITEM_WIDTH * 3.f / 4.f);
	ErGui::BetterDragFloat(ImVec4(66, 66, 255, 255), "##SkewY", &skewY, skewStep, skewStep * 5, "%.2f");

	ImGui::SameLine();
	if (ImGui::Button("S##Skew-Step"))
		ImGui::OpenPopup("SkewStepPopup");

	float skewXDelta = skewX - oldSkewX;
	float skewYDelta = skewY - oldSkewY;

	if (skewXDelta || skewYDelta) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		if (!skewObjectsPositionSnap) {
			skewFuncXGroup(objArr, skewXDelta, &skewX, &oldSkewX);
			skewFuncYGroup(objArr, skewYDelta, &skewY, &oldSkewY);
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				skewFuncX(obj, skewXDelta, &skewX, &oldSkewX);
				skewFuncY(obj, skewYDelta, &skewY, &oldSkewY);
			}
		}
	}

	renderCircleTool();
}

void ErGui::renderTransformation() {
	ImGui::Begin("Transform");


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
		s_resetStaticRotationScale = true;
	}
	
	ImGui::End();
}
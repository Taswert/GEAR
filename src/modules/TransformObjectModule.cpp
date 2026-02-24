#include "TransformObjectModule.hpp"
#include "CustomImGuiWidgets.hpp"
#include "myUtils.hpp"
#include <Geode/Enums.hpp>
#include <imgui.h>
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
	if (ImGui::CollapsingHeader("Circle Tool")) {
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

		if (ImGui::Button("Apply##Circle") && angleArc > 0.f && angleStep > 0.f) {

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

void renderStackTool() {
	if (ImGui::CollapsingHeader("Stack Tool")) {
		ImGui::Text("Copies");
		ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::DragInt("##CopiesStack", &stackCopies, 1, 1, 999, "%d")) {
			if (stackCopies < 1) stackCopies = 1;
		}
		
		ImGui::Separator();

		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("Position##XOffsetStack", &stackXOffset, 1.f, 5.f, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("##YOffsetStack", &stackYOffset, 1.f, 5.f, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));

		ImGui::Separator();

		// Rotate
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
		ErGui::DragFloat("Rotation##RotationStack", &stackRotate, 1.f, 5.f, "%.2f");

		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("X/Y##XRotationStack", &stackXRotate, 1.f, 5.f, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("##YRotationStack", &stackYRotate, 1.f, 5.f, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));

		ImGui::Separator();

		// Scale
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
		ErGui::DragFloat("Scale##ScaleStack", &stackScale, 0.1f, 0.5f, "%.2f", 0.1f);

		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("X/Y##XScaleStack", &stackXScale, 0.1f, 0.5f, "%.2f", 0.1f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
		ErGui::DragFloat("##YScaleStack", &stackYScale, 0.1f, 0.5f, "%.2f", 0.1f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));

		ImGui::Separator();

		// Z-Order
		ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
		int oldStackZOrder = stackZOrder;
		ErGui::DragInt("Z-Order##ZOrderStack", &stackZOrder, 1, 5, "%d", 1);


		// Perform func
		auto lel = GameManager::sharedState()->getEditorLayer();
		auto editorUI = lel->m_editorUI;

		if (ImGui::Button("Apply##Stack")) {
			if (editorUI->getSelectedObjects()) {
				lel->m_undoObjects->addObject(UndoObject::createWithArray(editorUI->getSelectedObjects(), UndoCommand::Select));
				auto* objs = CCArray::create();

				for (int i = 1; i <= stackCopies; ++i) {
					editorUI->onDuplicate(nullptr);
					auto selected = editorUI->getSelectedObjects();

					for (auto obj : CCArrayExt<GameObject*>(selected)) {
						editorUI->moveObject(obj, ccp(stackXOffset, stackYOffset));

						if ((stackRotate || stackXRotate || stackYRotate) && obj->canRotateFree()) {
							float rotX = obj->getRotationX();
							float rotY = obj->getRotationY();

							obj->setRotationX(rotX + stackXRotate + stackRotate);
							obj->setRotationY(rotY + stackYRotate + stackRotate);
						}

						if (stackScale || stackXScale || stackYScale) {
							float scaleX = obj->m_scaleX;
							float scaleY = obj->m_scaleY;

							obj->setScaleX(scaleX + stackXScale + stackScale);
							obj->setScaleY(scaleY + stackYScale + stackScale);
							obj->m_scaleX = scaleX + stackXScale + stackScale;
							obj->m_scaleY = scaleY + stackYScale + stackScale;
						}

						if (stackZOrder) {
							obj->m_zOrder += stackZOrder;
							if (obj->m_zOrder == 0) {
								if (stackZOrder > 0) 
									obj->m_zOrder++;
								else
									obj->m_zOrder--;
							}
						}
					}

					if (lel->m_undoObjects->count() > 0)
						lel->m_undoObjects->removeLastObject();

					objs->addObjectsFromArray(selected);
				}
				lel->m_undoObjects->addObject(UndoObject::createWithArray(objs, UndoCommand::Paste));

				editorUI->selectObjects(objs, true);
			}
		}
	}
}

void renderForObject(GameObject* obj) {
	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;

	float posX = obj->getRealPosition().x;
	float posY = obj->getRealPosition().y;

	float oldPosX = posX;
	float oldPosY = posY;

	bool isActive = false;
	bool isChanging = false;
	bool isClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left, false);

	const float INPUT_FLOAT_WIDTH = 30.f * 7.f + ImGui::GetStyle().ItemSpacing.x * 6.f;
	const float INNER_SPACING = ImGui::GetStyle().ItemInnerSpacing.x;

	static float moveStep = 	Mod::get()->getSavedValue<float>("move-step", 30.f);
	static float rotationStep = Mod::get()->getSavedValue<float>("rot-step", 90.f);
	static float scaleStep =	Mod::get()->getSavedValue<float>("scale-step", 0.5f);
	static float skewStep = 	Mod::get()->getSavedValue<float>("skew-step", 15.f);
	// Styling things
	//static ImVec2 minRect = ImGui::GetCursorPos();
	//static ImVec2 maxRect = ImGui::GetCursorPos();
	//auto dl = ImGui::GetWindowDrawList();

	//dl->AddRectFilled(minRect, maxRect, IM_COL32(50, 50, 50, 255), 6.f);

	//ImGui::BeginGroup();

	// ----- Position -----
	static bool moveStepPopupClosed = false;
	if (ImGui::BeginPopup("MoveStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("MoveStep", &moveStep, 1.f);

		if (ImGui::Button("0.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 0.5f;	ImGui::SameLine();
		if (ImGui::Button("2##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 2.f;		ImGui::SameLine();
		if (ImGui::Button("7.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 7.5f;	ImGui::SameLine();
		if (ImGui::Button("10##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("150##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 150.f;	ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		moveStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (moveStepPopupClosed) {
		moveStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("move-step", moveStep);
	}

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("Position##PosX", &posX, moveStep, moveStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) {
		isActive = true;
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("MoveStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##PosY", &posY, moveStep, moveStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) {
		isActive = true;
	}
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("MoveStepPopup");
	}

	float dPosX = posX - oldPosX;
	float dPosY = posY - oldPosY;

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (dPosX || dPosY) {
		auto rc = editorUI->m_rotationControl;
		rc->setPosition({ rc->getPositionX() + dPosX, rc->getPositionY() + dPosY });
		editorUI->moveObject(obj, { dPosX, dPosY });
		auto tc = editorUI->m_transformControl;
		tc->setPosition({tc->getPositionX() + dPosX, tc->getPositionY() + dPosY});
		tc->refreshControl();
	}

	// Reseting, so it would not cause any mistakes in undo list
	isActive = false;
	ImGui::Separator();



	// ----- Rotation -----
	static bool rotStepPopupClosed = false;
	if (ImGui::BeginPopup("RotationStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("RotStep", &rotationStep, 1.f);
		if (ImGui::Button("1##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 1.f;		ImGui::SameLine();
		if (ImGui::Button("10##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 10.f;	ImGui::SameLine();
		if (ImGui::Button("15##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 15.f;	ImGui::SameLine();
		if (ImGui::Button("30##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 30.f;	ImGui::SameLine();
		if (ImGui::Button("45##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 45.f;	ImGui::SameLine();
		if (ImGui::Button("90##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 90.f;	ImGui::SameLine();
		if (ImGui::Button("180##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 180.f;	ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		rotStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (rotStepPopupClosed) {
		rotStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("rot-step", rotationStep);
	}

	float rot = obj->getRotation();
	float rotX = obj->getRotationX();
	float rotY = obj->getRotationY();

	float oldRot = rot;
	float oldRotX = rotX;
	float oldRotY = rotY;

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	if (ErGui::DragFloat("Rotation##Rot", &rot, rotationStep, rotationStep * 5, "%.2f")) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotationStepPopup");
	}

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (isChanging) {
		rot = std::fmod(rot, 360);
		obj->setRotationX(std::fmod(rotX + rot - oldRot, 360));
		obj->setRotationY(std::fmod(rotY + rot - oldRot, 360));

		// auto tc = editorUI->m_transformControl;
		// tc->setRotation(tc->getRotation() + rot - oldRot);
		
		// Updating object hitbox
		obj->updateStartValues();
	}

	isActive = false;
	isChanging = false;


	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("X / Y##RotX", &rotX, rotationStep, rotationStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255))) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotationStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("##RotY", &rotY, rotationStep, rotationStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255))) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotationStepPopup");
	}

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (isChanging) {
		rotX = std::fmod(rotX, 360);
		rotY = std::fmod(rotY, 360);
		obj->setRotationY(rotY);
		obj->setRotationX(rotX);

		// Updating object hitbox
		obj->updateStartValues();
	}

	isActive = false;
	isChanging = false;
	ImGui::Separator();



	// ----- Scale -----
	static bool scaleStepPopupClosed = false;
	if (ImGui::BeginPopup("ScaleStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
		if (ImGui::Button("0.01##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.01f;		ImGui::SameLine();
		if (ImGui::Button("0.05##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.05f;		ImGui::SameLine();
		if (ImGui::Button("0.1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.1f;			ImGui::SameLine();
		if (ImGui::Button("0.25##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.25f;		ImGui::SameLine();
		if (ImGui::Button("0.5##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.5f;			ImGui::SameLine();
		if (ImGui::Button("1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("2##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 2.f;			ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		scaleStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (scaleStepPopupClosed) {
		scaleStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("scale-step", scaleStep);
	}

	float scale = obj->m_scaleX;
	float scaleX = obj->m_scaleX;
	float scaleY = obj->m_scaleY;

	float oldScale = scale;
	float oldScaleX = scaleX;
	float oldScaleY = scaleY;

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	if (ErGui::DragFloat("Scale##Scale", &scale, scaleStep, scaleStep * 5, "%.2f", 0.01f)) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (isChanging) {
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

		// Updating object hitbox
		obj->updateCustomScaleX(obj->m_scaleX);
		obj->updateCustomScaleY(obj->m_scaleY);
	}

	isActive = false;
	isChanging = false;

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("X / Y##ScaleX", &scaleX, scaleStep, scaleStep * 5, "%.2f", 0.01f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255))) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	if (ErGui::DragFloat("##ScaleY", &scaleY, scaleStep, scaleStep * 5, "%.2f", 0.01f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255))) isChanging = true;
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (isChanging) {
		short flipX = obj->isFlipX() ? -1 : 1;
		short flipY = obj->isFlipY() ? -1 : 1;
		obj->setScaleX(scaleX * flipX);
		obj->m_scaleX = scaleX;
		obj->setScaleY(scaleY * flipY);
		obj->m_scaleY = scaleY;

		// Updating object hitbox
		obj->updateCustomScaleX(obj->m_scaleX);
		obj->updateCustomScaleY(obj->m_scaleY);
	}


	isActive = false;
	isChanging = false;
	ImGui::Separator();



	// ----- Skew -----
	static bool skewStepPopupClosed = false;
	if (ImGui::BeginPopup("SkewStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 1.f;		ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 5.f;		ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 45.f;		ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		skewStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (skewStepPopupClosed) {
		skewStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("skew-step", skewStep);
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

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("Skew##SkewX", &skewX, skewStep, skewStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("SkewStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##SkewY", &skewY, skewStep, skewStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("SkewStepPopup");
	}

	float skewXDelta = skewX - oldSkewX;
	float skewYDelta = skewY - oldSkewY;

	if (isActive && isClicked) {
		addObjectToUndoList(obj, UndoCommand::Transform);
	}

	if (skewXDelta) {
		skewFuncX(obj, skewXDelta, &skewX, &oldSkewX);
	}
	if (skewYDelta) {
		skewFuncY(obj, skewYDelta, &skewY, &oldSkewY);
	}
	ImGui::Separator();


	// ----- Flip -----
	const float additiveSpace = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x * 2;

	ImGui::Text("Flip");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	if (ImGui::Button("Horizontal", ImVec2(ErGui::SHORT_INPUT_ITEM_WIDTH, 0))) {
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setFlipX(!obj->m_isFlipX);
	}
	ImGui::SameLine();
	if (ImGui::Button("Vertical", ImVec2(ErGui::SHORT_INPUT_ITEM_WIDTH, 0))) {
		addObjectToUndoList(obj, UndoCommand::Transform);
		obj->setFlipY(!obj->m_isFlipY);
	}
	

	ImGui::EndGroup();
	
	// Also styling things. Gonna make post release.
	//minRect = ImGui::GetItemRectMin();
	//maxRect = {ImGui::GetWindowContentRegionMax().x + minRect.x, ImGui::GetItemRectMax().y};

	renderCircleTool();
	renderStackTool();
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


	bool isActive = false;
	bool isChanging = false;
	bool isClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left, false);

	const float INPUT_FLOAT_WIDTH = 30.f * 7.f + ImGui::GetStyle().ItemSpacing.x * 6.f;
	const float INNER_SPACING = ImGui::GetStyle().ItemInnerSpacing.x;


	static float moveStep = 	Mod::get()->getSavedValue<float>("move-step", 30.f);
	static float rotationStep = Mod::get()->getSavedValue<float>("rot-step", 90.f);
	static float scaleStep =	Mod::get()->getSavedValue<float>("scale-step", 0.5f);
	static float skewStep = 	Mod::get()->getSavedValue<float>("skew-step", 15.f);
	

	// ----- Position -----
	static bool moveStepPopupClosed = false;
	if (ImGui::BeginPopup("MoveStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("MoveStep", &moveStep, 1.f);
		if (ImGui::Button("0.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 0.5f;	ImGui::SameLine();
		if (ImGui::Button("2##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 2.f;		ImGui::SameLine();
		if (ImGui::Button("7.5##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 7.5f;	ImGui::SameLine();
		if (ImGui::Button("10##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 10.f;	ImGui::SameLine();
		if (ImGui::Button("15##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 15.f;	ImGui::SameLine();
		if (ImGui::Button("30##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 30.f;	ImGui::SameLine();
		if (ImGui::Button("150##MoveStepPreset", ImVec2({ 30.f, 0.f }))) moveStep = 150.f;	ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		moveStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (moveStepPopupClosed) {
		moveStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("move-step", moveStep);
	}


	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("Position##PosX", &groupCenter.x, moveStep, moveStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("MoveStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##PosY", &groupCenter.y, moveStep, moveStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("MoveStepPopup");
	}
	ImGui::Separator();

	if (isActive && isClicked) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
	}

	float posXDelta = groupCenter.x - oldGroupCenter.x;
	float posYDelta = groupCenter.y - oldGroupCenter.y;
	if (posXDelta || posYDelta) {
		auto rc = editorUI->m_rotationControl;
		rc->setPosition({ rc->getPositionX() + posXDelta, rc->getPositionY() + posYDelta });
		for (auto obj : CCArrayExt<GameObject*>(objArr)) {
			editorUI->moveObject(obj, { posXDelta, posYDelta });
		}

		auto tc = editorUI->m_transformControl;
		tc->setPosition({tc->getPositionX() + posXDelta, tc->getPositionY() + posYDelta});
		tc->refreshControl();
	}

	isActive = false;



	// ----- Rotation -----
	static bool rotStepPopupClosed = false;
	if (ImGui::BeginPopup("RotStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("RotStep", &rotationStep, 1.f);
		if (ImGui::Button("1##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 1.f;		ImGui::SameLine();
		if (ImGui::Button("10##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 45.f;		ImGui::SameLine();
		if (ImGui::Button("90##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 90.f;		ImGui::SameLine();
		if (ImGui::Button("180##RotateStepPreset", ImVec2({ 30.f, 0.f }))) rotationStep = 180.f;	ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		rotStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (rotStepPopupClosed) {
		rotStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("rot-step", rotationStep);
	}

	// -- All Rotation
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::DragFloat("Rotation##Rotation", &rotation, rotationStep, rotationStep * 5, "%.2f");
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotStepPopup");
	}
	ImGui::SameLine();
	ImGui::Checkbox("##RotPosSnap", &rotateObjectsPositionSnap);
	ImGui::SetItemTooltip("Position Snap");

	// -- XY Rotation
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("X / Y##RotationX", &rotationX, rotationStep, rotationStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##RotationY", &rotationY, rotationStep, rotationStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("RotStepPopup");
	}
	
	ImGui::Separator();

	if (isActive && isClicked) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
	}

	// -- Apply Rotation
	float rotDelta = rotation - oldRotation;
	float rotDeltaX = rotationX - oldRotationX;
	float rotDeltaY = rotationY - oldRotationY;
	if (rotDelta || rotDeltaX || rotDeltaY) {
		// auto tc = editorUI->m_transformControl;
		// tc->setRotation(tc->getRotation() + rotDelta);
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

				// Updating object hitbox
				obj->updateStartValues();
			}
		}
	}

	isActive = false;



	// ----- Scale -----
	static bool scaleStepPopupClosed = false;
	if (ImGui::BeginPopup("ScaleStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("ScaleStep", &scaleStep, 0.25f);
		if (ImGui::Button("0.01##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.01f;	ImGui::SameLine();
		if (ImGui::Button("0.05##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.05f;	ImGui::SameLine();
		if (ImGui::Button("0.1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.1f;		ImGui::SameLine();
		if (ImGui::Button("0.25##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.25f;	ImGui::SameLine();
		if (ImGui::Button("0.5##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 0.5f;		ImGui::SameLine();
		if (ImGui::Button("1##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("2##ScaleStepPreset", ImVec2({ 30.f, 0.f }))) scaleStep = 2.f;			ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		scaleStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (scaleStepPopupClosed) {
		scaleStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("scale-step", scaleStep);
	}

	// -- All Scale
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH * 2.f + ImGui::GetStyle().ItemSpacing.x);
	ErGui::DragFloat("Scale##Scale", &scale, scaleStep, scaleStep * 5, "%.2f", 0.01f);
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}
	ImGui::SameLine();
	ImGui::Checkbox("##ScalePosSnap", &scaleObjectsPositionSnap);
	ImGui::SetItemTooltip("Position Snap");

	// -- XY Scale
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("X / Y##ScaleX", &scaleX, scaleStep, scaleStep * 5, "%.2f", 0.01f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##ScaleY", &scaleY, scaleStep, scaleStep * 5, "%.2f", 0.01f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("ScaleStepPopup");
	}

	ImGui::Separator();

	if (isActive && isClicked) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
	}

	float scaleDelta = scale - oldScale;
	float scaleDeltaX = scaleX - oldScaleX;
	float scaleDeltaY = scaleY - oldScaleY;

	
	if (scaleDeltaX || scaleDeltaY || scaleDelta) {
		groupCenter = editorUI->getGroupCenter(objArr, false);
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

				// Updating object hitbox
				obj->updateCustomScaleX(obj->m_scaleX);
				obj->updateCustomScaleY(obj->m_scaleY);
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

				// Updating object hitbox
				obj->updateCustomScaleX(obj->m_scaleX);
				obj->updateCustomScaleY(obj->m_scaleY);
			}
		}
	}

	isActive = false;



	// ----- Skew -----
	static bool skewStepPopupClosed = false;
	if (ImGui::BeginPopup("SkewStepPopup")) {
		ImGui::SetNextItemWidth(INPUT_FLOAT_WIDTH);
		ImGui::InputFloat("SkewStep", &skewStep, 1.f);
		if (ImGui::Button("1##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 1.f;			ImGui::SameLine();
		if (ImGui::Button("5##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 5.f;			ImGui::SameLine();
		if (ImGui::Button("10##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 10.f;		ImGui::SameLine();
		if (ImGui::Button("15##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 15.f;		ImGui::SameLine();
		if (ImGui::Button("30##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 30.f;		ImGui::SameLine();
		if (ImGui::Button("45##SkewStepPreset", ImVec2({ 30.f, 0.f }))) skewStep = 45.f;		ImGui::SameLine(0.f, INNER_SPACING);
		ImGui::Text("Presets");
		skewStepPopupClosed = true;
		ImGui::EndPopup();
	}
	else if (skewStepPopupClosed) {
		skewStepPopupClosed = false;
		Mod::get()->setSavedValue<float>("skew-step", skewStep);
	}

	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("Skew##SkewX", &skewX, skewStep, skewStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(255, 66, 66, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("SkewStepPopup");
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ErGui::SHORT_INPUT_ITEM_WIDTH);
	ErGui::DragFloat("##SkewY", &skewY, skewStep, skewStep * 5, "%.2f", 1.f, 0.f, 0.f, 0, false, ImVec4(66, 66, 255, 255));
	if (ImGui::IsItemActive()) isActive = true;
	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("SkewStepPopup");
	}

	if (isActive && isClicked) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
	}

	float skewXDelta = skewX - oldSkewX;
	float skewYDelta = skewY - oldSkewY;

	if (skewXDelta || skewYDelta) {
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
	ImGui::Separator();


	// ----- Flip -----
	const float additiveSpace = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x * 2;

	ImGui::Text("Flip");
	ImGui::SameLine(ErGui::FIRST_ELEMENT_SAMELINE_SPACING);
	if (ImGui::Button("Horizontal", ImVec2(ErGui::SHORT_INPUT_ITEM_WIDTH, 0))) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		if (!flipObjectsPositionSnap) {
			editorUI->flipObjectsX(objArr);
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->setFlipX(!obj->m_isFlipX);
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Vertical", ImVec2(ErGui::SHORT_INPUT_ITEM_WIDTH, 0))) {
		addObjectsToUndoList(objArr, UndoCommand::Transform);
		if (!flipObjectsPositionSnap) {
			editorUI->flipObjectsY(objArr);
		}
		else {
			for (auto obj : CCArrayExt<GameObject*>(objArr)) {
				obj->setFlipY(!obj->m_isFlipY);
			}
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox("##FlipPosSnap", &flipObjectsPositionSnap);
	ImGui::SetItemTooltip("Position Snap");

	renderCircleTool();
	renderStackTool();
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
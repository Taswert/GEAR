#include "ToolsModule.hpp"

void parseObjects(const char* tabName, EditorUI* editorUI) {
	auto bsl = static_cast<BoomScrollLayer*>(editorUI->getChildByID(tabName)->getChildren()->objectAtIndex(0));
	auto el = static_cast<ExtendedLayer*>(bsl->getChildren()->objectAtIndex(0));
	for (ButtonPage* bp : CCArrayExt<ButtonPage*>(el->getChildren())) {
		auto menu = static_cast<CCMenu*>(bp->getChildren()->objectAtIndex(0));
		for (CreateMenuItem* cmi : CCArrayExt<CreateMenuItem*>(menu->getChildren())) {
			auto bs = static_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
			for (CCNode* gameObject : CCArrayExt<CCNode*>(bs->getChildren())) {
				if (dynamic_cast<GameObject*>(gameObject)) {
					std::cout << dynamic_cast<GameObject*>(gameObject)->m_objectID << ", ";
				}
			}
		}
	}
}

void EditorUI_toggleSpecialEditButtons(EditorUI* eui) {
	return reinterpret_cast<void(__thiscall*)(EditorUI*)>(geode::base::get() + 0x119010)(eui);
}

void ErGui::renderToolsModule1() {
	ImGui::Begin("Tools-Module1");
	ErGui::enableClicks();
		
	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;

	ImGui::RadioButton("Build##RADIO",	&editorUI->m_selectedMode, 2);
	ImGui::RadioButton("Edit##RADIO",	&editorUI->m_selectedMode, 3);
	ImGui::RadioButton("Delete##RADIO",	&editorUI->m_selectedMode, 1);
	ImGui::RadioButton("Zoom##RADIO",	&editorUI->m_selectedMode, 4);
	ImGui::RadioButton("Image##RADIO",	&editorUI->m_selectedMode, 5);

	ImGui::Text("-");

		
		
	bool swipeBool = editorUI->m_swipeEnabled;
	if (ImGui::Checkbox("Swipe", &swipeBool)) {
		editorUI->m_swipeEnabled = swipeBool;

		if (isLassoEnabled && editorUI->m_swipeEnabled) {
			lassoPatch->enable();
		}
		else {
			lassoPatch->disable();
		}
	}
		
	bool rotationBool = gameManager->getGameVariable("0007");
	if (ImGui::Checkbox("Rotation", &rotationBool)) {
		gameManager->setGameVariable("0007", rotationBool);
		if (rotationBool) {
			EditorUI_toggleSpecialEditButtons(editorUI);
		}
		else {
			editorUI->m_rotationControl->setVisible(false);
		}
	}

	bool freeMoveBool = editorUI->m_freeMoveEnabled;
	if (ImGui::Checkbox("Free Move", &freeMoveBool)) {
		editorUI->m_freeMoveEnabled = freeMoveBool;
	}

	bool snapBool = gameManager->getGameVariable("0008");
	if (ImGui::Checkbox("Snap", &snapBool)) {
		gameManager->setGameVariable("0008", snapBool);
	}
		
	ImGui::Text("-");

	if (ImGui::Button("Warp")) {
		editorUI->activateTransformControl(nullptr);
	}

	if (ImGui::Button("De-Select")) {
		editorUI->deselectAll();
	}

	if (ImGui::Button("Delete")) {
		if (editorUI->m_selectedObject) editorUI->deleteObject(editorUI->m_selectedObject, false);
		else if (editorUI->m_selectedObjects->count() > 0) editorUI->onDeleteSelected(nullptr);
	}

	ImGui::Text("-");

	auto mode = *reinterpret_cast<int*>(reinterpret_cast<long long>(editorUI->m_editorLayer) + 0x878);
	std::string playtestStr = "Playtest";
	if (mode == 1) playtestStr = "Pause Playtest";

	if (ImGui::Button(playtestStr.c_str())) {
		editorUI->onPlaytest(nullptr);
	}

	if (ImGui::Button("Playback")) {
		editorUI->onPlayback(nullptr);
	}

	if (mode == 1 || mode == 2) {
		if (ImGui::Button("Stop")) {
			editorUI->onStopPlaytest(nullptr);
		}
	}

	ImGui::End();
}

int colorSelectImGuiPopup(int colorId, int oneortwo) {
	auto popupStr = std::string("colorSelectPopup") + std::to_string(oneortwo);
	if (ImGui::BeginPopup(popupStr.c_str())) {
		for (int i = 0; i < 1102; i++) {
			auto effectManager = GameManager::sharedState()->m_levelEditorLayer->m_levelSettings->m_effectManager;
			auto ccMyColor = effectManager->getColorAction(i);

			std::string suffix = "";
			
			switch (i) {
				default:
					break;
			case 1000:
				suffix = "-BG";
				break;
			case 1001:
				suffix = "-G1";
				break;
			case 1002:
				suffix = "-LINE";
				break;
			case 1003:
				suffix = "-3DL";
				break;
			case 1004:
				suffix = "-OBJECT";
				break;
			case 1005:
				suffix = "-P1";
				break;
			case 1006:
				suffix = "-P2";
				break;
			case 1007:
				suffix = "-LBG";
				break;
			case 1009:
				suffix = "-G2";
				break;
			case 1010:
				suffix = "-BLACK";
				break;
			case 1011:
				suffix = "-WHITE";
				break;
			case 1012:
				suffix = "-LIGHTER";
				break;
			case 1013:
				suffix = "-MG";
				break;
			case 1014:
				suffix = "-MG2";
				break;
			}
			
			std::string btnStr = std::string("color") + std::to_string(i) + suffix + std::string("##COLOR-BUTTON-POPUP");

			//std::cout << btnStr << "\n";
			if (ImGui::ColorButton(btnStr.c_str(), ImVec4(ccMyColor->m_fromColor.r / 255.f, ccMyColor->m_fromColor.g / 255.f, ccMyColor->m_fromColor.b / 255.f, 1.f))) {
				colorId = i;
				ImGui::CloseCurrentPopup();
			}

			auto colEditPopupStr = btnStr + "-EDIT-POPUP";
			if (ImGui::BeginPopup(colEditPopupStr.c_str())) {
				auto colEditStr = btnStr + "-EDIT";
				float editedColors[3] = { ccMyColor->m_fromColor.r / 255.f, ccMyColor->m_fromColor.g / 255.f, ccMyColor->m_fromColor.b / 255.f };
				ImGui::ColorEdit3(colEditStr.c_str(), editedColors);
				ccMyColor->m_fromColor = ccColor3B(int(editedColors[0] * 255), int(editedColors[1] * 255), int(editedColors[2] * 255));

				//std::cout << int(effectManager->getColorAction(i)->m_fromColor.r) << " " << int(effectManager->getColorAction(i)->m_fromColor.g) << " " << int(effectManager->getColorAction(i)->m_fromColor.b) << "\n";
				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup(colEditPopupStr.c_str());
			}

			if ((i+1) % 10 != 0) ImGui::SameLine();
		}
		ImGui::EndPopup();
	}
	return colorId;
}

void ErGui::renderToolsModule2() {
	ImGui::Begin("Tools-Module2");

	auto gameManager = GameManager::sharedState();
	auto editorUI = gameManager->getEditorLayer()->m_editorUI;

	switch (editorUI->m_selectedMode) {
		case 1: //Delete
		{
			if (ImGui::Button("DeleteAllSP")) {
				editorUI->onDeleteStartPos(nullptr);
			}

			if (auto obj = editorUI->m_selectedObject) {
				std::string buttonStr = "DeleteAll-" + std::to_string(obj->m_objectID);
				if (ImGui::Button(buttonStr.c_str())) {
					editorUI->onDeleteSelectedType(nullptr);
				}
			}

			ImGui::Button("##OBJECT", { 30.f, 30.f });

			std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(editorUI->m_selectedObjectIndex);
			if (!newFrameName.empty()) {
				CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
				if (frame)
					ErGui::drawImageInImGui(frame);
			}

			int deleteFilterMode = GameManager::sharedState()->getIntGameVariable("0005");
			GameManager::sharedState()->setIntGameVariable("0006", editorUI->m_selectedObjectIndex);
			std::string customObjectStr = "Custom-" + std::to_string(GameManager::sharedState()->getIntGameVariable("0006")) + std::string("##RADIO");

			ImGui::RadioButton("None##RADIO",			&deleteFilterMode, 0);
			ImGui::RadioButton("Details##RADIO",		&deleteFilterMode, 1);
			ImGui::RadioButton("Static##RADIO",			&deleteFilterMode, 2);
			ImGui::RadioButton(customObjectStr.c_str(), &deleteFilterMode, 3);

			GameManager::sharedState()->setIntGameVariable("0005", deleteFilterMode);
			break;
		}
		case 2: //Build
		{
			int color1 = Mod::get()->template getSavedValue<int>("build-color-1");
			int color2 = Mod::get()->template getSavedValue<int>("build-color-2");

			auto effectManager = gameManager->m_levelEditorLayer->m_effectManager;
			auto ccMyColor1 = effectManager->getColorSprite(color1);
			if (ImGui::ColorButton("color1##COLOR-BUTTON", ImVec4(ccMyColor1->m_color.r / 255.f, ccMyColor1->m_color.g / 255.f, ccMyColor1->m_color.b / 255.f, 1.f))) {
				ImGui::OpenPopup("colorSelectPopup1");
			}
			color1 = colorSelectImGuiPopup(color1, 1);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
			ImGui::InputInt("##BUILD-COLOR-1", &color1);
			if (color1 > 1101) color1 = 1101;
			if (color1 < 0) color1 = 0;

			ImGui::SameLine();
			bool enableColorBuild1 = Mod::get()->getSavedValue<bool>("enable-build-color-1");
			if (ImGui::Checkbox("##ENABLE-COLOR-BUILD-1", &enableColorBuild1)) {
				Mod::get()->setSavedValue("enable-build-color-1", enableColorBuild1);
			}



			auto ccMyColor2 = effectManager->getColorSprite(color2);
			if (ImGui::ColorButton("color2##COLOR-BUTTON", ImVec4(ccMyColor2->m_color.r / 255.f, ccMyColor2->m_color.g / 255.f, ccMyColor2->m_color.b / 255.f, 1.f))) {
				ImGui::OpenPopup("colorSelectPopup2");
			}
			color2 = colorSelectImGuiPopup(color2, 2);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.f);
			ImGui::InputInt("##BUILD-COLOR-2", &color2);
			if (color2 > 1101) color2 = 1101;
			if (color2 < 0) color2 = 0;

			ImGui::SameLine();
			bool enableColorBuild2 = Mod::get()->getSavedValue<bool>("enable-build-color-2");
			if (ImGui::Checkbox("##ENABLE-COLOR-BUILD-2", &enableColorBuild2)) {
				Mod::get()->setSavedValue("enable-build-color-2", enableColorBuild2);
			}

			Mod::get()->setSavedValue("build-color-1", color1);
			Mod::get()->setSavedValue("build-color-2", color2);


			ImGui::Button("##SELECTED-OBJECT", { 30.f, 30.f });

			std::string newFrameName = ObjectToolbox::sharedState()->intKeyToFrame(editorUI->m_selectedObjectIndex);
			if (!newFrameName.empty()) {
				CCSpriteFrame* frame = CCSpriteFrameCache::get()->spriteFrameByName(newFrameName.c_str());
				if (frame) 
					ErGui::drawImageInImGui(frame);
			}

			break;
		}
		case 3: //Edit
		{
			if (ImGui::Checkbox("Lasso", &isLassoEnabled)) {
				if (isLassoEnabled && editorUI->m_swipeEnabled) {
					//GameManager::sharedState()->setGameVariable("0003", true);
					lassoPatch->enable();
				}
				else {
					//GameManager::sharedState()->setGameVariable("0003", false);
					lassoPatch->disable();
				}
			}

			int selectMode = Mod::get()->getSavedValue<int>("select-mode");

			ImGui::RadioButton("Additive##RADIO",		&selectMode, 1);
			ImGui::RadioButton("Subtractive##RADIO",	&selectMode, 2);
			ImGui::RadioButton("Intersective##RADIO",	&selectMode, 3);

			Mod::get()->setSavedValue("select-mode", selectMode);
			break;
		}
		case 4: //Zoom
		{
			if (ImGui::Button("Zoom+")) editorUI->zoomIn(nullptr);
			if (ImGui::Button("Zoom-")) editorUI->zoomOut(nullptr);
			float zoomMul = Mod::get()->template getSavedValue<float>("zoom-multiplier");
			if (ImGui::DragFloat("Zoom Mul", &zoomMul, 0.1f) && zoomMul >= 0.1f && zoomMul <= 10.f) {
				Mod::get()->setSavedValue("zoom-multiplier", zoomMul);
				editorUI->updateGridNodeSize();
			}
			float gridSize = Mod::get()->template getSavedValue<float>("grid-size");
			if (ImGui::DragFloat("Grid Size", &gridSize, 0.5f) && gridSize >= 0.1f) {
				Mod::get()->setSavedValue("grid-size", gridSize);
				editorUI->updateGridNodeSize();
			}
			break;
		}
		case 5: //Image
		{

			break;
		}
		default:
			break;
	}

	ImGui::End();
}
